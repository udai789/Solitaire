//
//  GameMain.cpp
//  Solitaire
//
//  Created by kk on 2015/08/10.
//
//

#include "GameMain.h"

const Rect TALON_AREA_RECT=Rect(0,970,310,160);//山札の領域
const Rect HOME_CELL_AREA_RECT=Rect(310,970,420,160);//ホームセルの領域
const Rect BOARD_CARD_AREA_RECT=Rect(0,0,720,970);//場札の領域

const Vec2 HELP_SELECT=Vec2(220,80);//help_selectの位置
const Vec2 HELP_SELECT_DIFF=Vec2(100,0);//help_selectの差分

const Vec2 TALON_DIFF=Vec2(0,-60);//カードの重なり位置の差
const Size CARD_SIZE=Size(100,150);

const int TIME_BONUS=700000;

const char* PULLCOUNT_KEY="pullCountKey";//山札からカードを引く枚数の記録
const char* HIGHSCORE_KEY="highScoreKey";//ハイスコアの記録
const char* DRAG_SCHEDULE="dragSchedule";//ドラッグ時のスケジュールkey

const float TOUCH_TIME_PERIOD=0.5;//test ダブルタッチの判定時間

GameMain::GameMain()
:_gameState(GameState::START)
,_time(0)
,_score(0)
,_tenCount(0)
,_moveLayer(nullptr)
,_dragLayer(nullptr)
,_talonLayer(nullptr)
,_boardCardLayer(nullptr)
,_homeCellLayer(nullptr)
,_timeLabel(nullptr)
,_scoreLabel(nullptr)
,_endLayer(nullptr)
,_touchTime(0)
,_doubleTouchFlag(false)
,_oneTouch(false)
{
    random_device rdev;
    _engine.seed(rdev());
}

GameMain::~GameMain()
{
    CC_SAFE_RELEASE_NULL(_moveLayer);
    CC_SAFE_RELEASE_NULL(_dragLayer);
    CC_SAFE_RELEASE_NULL(_talonLayer);
    CC_SAFE_RELEASE_NULL(_boardCardLayer);
    CC_SAFE_RELEASE_NULL(_homeCellLayer);
    CC_SAFE_RELEASE_NULL(_timeLabel);
    CC_SAFE_RELEASE_NULL(_scoreLabel);
    CC_SAFE_RELEASE_NULL(_endLayer);
}

Scene* GameMain::createScene()
{
    auto scene=Scene::create();
    auto layer=GameMain::create();
    
    scene->addChild(layer);
    return scene;
}

bool GameMain::init()
{
    if(!Layer::init()){return false;}
    
    createBackground();
    createLabel();
    createCards();
    createBoards();
    createMenuButton();
    settingStart();
    
    auto listener=EventListenerTouchOneByOne::create();
    //ドラッグ開始
    listener->onTouchBegan=[this](Touch* touch,Event* event){
        ////
        if(!_moveCards.empty()){
            if(_moveCards.front()->getNumberOfRunningActions()){//ダブルタッチアニメーション中
                auto card=_moveCards.front();
                card->stopAllActions();//アニメーションをキャンセル
                this->dragTouchEnded(_homeCellLayer->getHomeCellTalonPosition(card->getCardType()));//CallFuncの処理を実行
            }
        }
        ////
        
        auto position=touch->getLocation();
        if(_gameState==GameState::TOUCH_WAITING){
            _moveCards.clear();
            if(TALON_AREA_RECT.containsPoint(position)){
                _moveCards=_talonLayer->dragCards(position);
                this->setDragLayer(_talonLayer);
            }else if(BOARD_CARD_AREA_RECT.containsPoint(position)){
                _moveCards=_boardCardLayer->dragCards(position);
                this->setDragLayer(_boardCardLayer);
            }else if(HOME_CELL_AREA_RECT.containsPoint(position)){
                _moveCards=_homeCellLayer->dragCards(position);
                this->setDragLayer(_homeCellLayer);
            }
            
            if(!_moveCards.empty()){//カードがドラッグできれば
                auto cardPosition=Vec2(0,-CARD_SIZE.height/4);
                int z=1;
                for(auto card:_moveCards){
                    card->setPosition(cardPosition);
                    _moveLayer->addChild(card,z);
                    cardPosition+=TALON_DIFF;
                    z++;
                }
                
                _moveLayer->setPosition(position);
                
                _gameState=GameState::DRAG;
                
                //ダブルタップ処理
                _doubleTouchFlag=false;
                if(_dragLayer==_talonLayer || _dragLayer==_boardCardLayer){//山札あるいは場札からのドロップ
                    if(_homeCellLayer->checkDropCard(_moveCards)){//ホームセルに置けるカードである
                        _doubleTouchFlag=true;//ダブルタップ処理フラグをtrue
                        _oneTouch=false;//一回タップされたかのフラグをfalse
                        _touchTime=0;//タップ時間の初期化
                    }
                }
            }else{
                this->setDragLayer(nullptr);
            }
        }
        return true;
    };
    //ドロップ、山札の切り替え
    listener->onTouchEnded=[this](Touch* touch,Event* event){
        auto position=touch->getLocation();
        if(_gameState==GameState::TOUCH_WAITING){
            if(TALON_AREA_RECT.containsPoint(position)){
                if(_talonLayer->touchTalonCard(position)){
                    //山札が一巡
                    if(_talonLayer->getPullCount()==3){
                        //引くカードが3枚ずつの場合-20
                        this->updateScore(-20);
                    }else if(_talonLayer->getPullCount()==1){
                        //引くカードが1枚ずつの場合-100
                        this->updateScore(-100);
                    }else{
                        log("Talon::_pullCoount neither 1 nor 3.");
                    }
                }
            }
        }else if(_gameState==GameState::DRAG){
            if(_doubleTouchFlag){//ダブルタップを行う可能性がある
                if(_oneTouch){//既に一度タップされている
                    if(_touchTime<TOUCH_TIME_PERIOD){//ダブルタップ受付時間以内
                        this->unschedule(DRAG_SCHEDULE);
                        auto card=_moveCards.front();
                        //this->dragTouchEnded(_homeCellLayer->getHomeCellTalonPosition(card->getCardType()));
                        _doubleTouchFlag=false;
                        
                        ////
                        //カードを移すホームセルの位置
                        auto homeCellPosition=_homeCellLayer->getHomeCellTalonPosition(card->getCardType());
                        //_moveLayerとGameMainレイヤーは基準座標が違うためconvertする
                        card->runAction(Sequence::create(MoveTo::create(0.3,_moveLayer->convertToNodeSpace(homeCellPosition))
                                                         ,CallFunc::create([this,homeCellPosition](){
                            this->dragTouchEnded(homeCellPosition);
                        })
                                                         ,NULL));
                        ////
                    }
                }else{
                    if(_touchTime<TOUCH_TIME_PERIOD){//ダブルタップ受付時間以内
                        _oneTouch=true;
                        //ダブルタップされなかったときのためにスケジュールを設定
                        this->scheduleOnce([this,position](float dt){
                            this->dragTouchEnded(position);
                            _doubleTouchFlag=false;
                        },TOUCH_TIME_PERIOD-_touchTime+0.1,DRAG_SCHEDULE);
                    }else{
                        this->dragTouchEnded(position);
                        _doubleTouchFlag=false;
                    }
                }
            }else{
                this->dragTouchEnded(position);
            }
        }
    };
    //ドロップ中の移動
    listener->onTouchMoved=[this](Touch* touch,Event* event){
        auto position=touch->getLocation();
        if(_gameState==GameState::DRAG){
            _moveLayer->setPosition(position);
        }
    };
    //キャンセル
    listener->onTouchCancelled=[this](Touch* touch,Event* event){
        if(_gameState==GameState::DRAG){
            _moveLayer->removeAllChildren();
            _dragLayer->cancelCards(_moveCards);
            _gameState=GameState::TOUCH_WAITING;
        }
    };
    //test
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);
    
    return true;
}

void GameMain::createBackground()
{
    auto winSize=Director::getInstance()->getWinSize();
    
    auto background=Sprite::create("board.png");
    background->setPosition(winSize/2);
    
    this->addChild(background,convertLayerZPositionIntoInt(LayerZPosition::BACKGROUND));
}

void GameMain::createCards()
{
    _cards.clear();
    for(int i=1;i<=13;i++){
        _cards.pushBack(Card::createWithTypeANumber(Card::CardType::HEART,i));
        _cards.pushBack(Card::createWithTypeANumber(Card::CardType::SPADE,i));
        _cards.pushBack(Card::createWithTypeANumber(Card::CardType::DIAMOND,i));
        _cards.pushBack(Card::createWithTypeANumber(Card::CardType::CLUB,i));
    }
    
    shuffleCards();
}

void GameMain::shuffleCards()
{
    Vector<Card*> cards;
    
    //カードをシャッフル
    while(!_cards.empty()){
        auto count=static_cast<int>(_cards.size()-1);//添字の最大値
        auto index=generateRandomInt(0,count);
        auto card=_cards.at(index);
        cards.pushBack(card);
        _cards.eraseObject(card);
    }
    
    _cards=cards;
}

void GameMain::createBoards()
{
    Vector<Card*> cards=_cards;
    //場札
    Vector<Card*> boardCardCards;
    for(int i=0;i<28;i++){//場札用の28枚を取り出す
        boardCardCards.pushBack(cards.back());
        cards.popBack();
    }
    setBoardCardLayer(BoardCard::createWithCards(boardCardCards));
    this->addChild(_boardCardLayer,convertLayerZPositionIntoInt(LayerZPosition::BOARDS));
    
    //山札
    int pullCount=UserDefault::getInstance()->getIntegerForKey(PULLCOUNT_KEY,3);
    setTalonLayer(Talon::createWithCardsAPullCount(cards,pullCount));
    this->addChild(_talonLayer,convertLayerZPositionIntoInt(LayerZPosition::BOARDS));
    
    //組札
    setHomeCellLayer(HomeCell::create());
    this->addChild(_homeCellLayer,convertLayerZPositionIntoInt(LayerZPosition::BOARDS));
    
    //移動用
    auto layer=Layer::create();
    setMoveLayer(layer);
    this->addChild(layer,convertLayerZPositionIntoInt(LayerZPosition::MOVE));
}

int GameMain::generateRandomInt(int min,int max)
{
    uniform_int_distribution<int> dest(min,max);
    return dest(_engine);
}

void GameMain::createLabel()
{
    auto timeSubLabel=Label::createWithSystemFont("TIME","Marker Felt",36);
    timeSubLabel->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    timeSubLabel->setPosition(Vec2(200,1200));
    this->addChild(timeSubLabel);
    
    auto time=Label::createWithSystemFont("0","Marker Felt",36);
    time->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    time->setPosition(Vec2(200,1140));
    setTimeLabel(time);
    this->addChild(time);
    
    auto scoreSub=Label::createWithSystemFont("SCORE","Marker Felt",36);
    scoreSub->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    scoreSub->setPosition(Vec2(510,1200));
    this->addChild(scoreSub);
    
    auto score=Label::createWithSystemFont("0","Marker Felt",36);
    score->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
    score->setPosition(Vec2(510,1140));
    setScoreLabel(score);
    this->addChild(score);
}

void GameMain::settingStart()
{
    _gameState=GameState::START;
    auto winSize=Director::getInstance()->getWinSize();
    
    auto layer=Layer::create();
    this->addChild(layer,convertLayerZPositionIntoInt(LayerZPosition::COVER));
    
    auto sprite=Sprite::create("TouchStart.png");
    sprite->setPosition(winSize/2);
    sprite->setOpacity(0);
    sprite->runAction(RepeatForever::create(Sequence::create(FadeIn::create(1)
                                                             ,FadeOut::create(1)
                                                             ,NULL)));
    
    layer->addChild(sprite);
    
    auto listener=EventListenerTouchOneByOne::create();
    listener->onTouchBegan=[](Touch* touch,Event* event){
        return true;
    };
    listener->onTouchEnded=[this,layer,sprite](Touch* touch,Event* event){
        layer->removeFromParent();
        _gameState=GameState::TOUCH_WAITING;
        this->scheduleUpdate();
    };
    
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,layer);
}

int GameMain::convertLayerZPositionIntoInt(GameMain::LayerZPosition type)
{
    return static_cast<int>(type);
}

void GameMain::update(float dt)
{
    Layer::update(dt);
    
    _time+=dt;
    _tenCount+=dt;
    //10秒経過で-2
    if(_tenCount>=10){
        _tenCount-=10;
        updateScore(-2);
    }
    _timeLabel->setString(StringUtils::format("%d",static_cast<int>(floor(_time))));
    
    //test
    if(_doubleTouchFlag){//ダブルタッチ判定中
        _touchTime+=dt;
    }
    //test
}

void GameMain::updateScore(int value)
{
    _score=max(0,_score+value);
    _scoreLabel->setString(StringUtils::format("%d",_score));
}

void GameMain::settingEnd()
{
    _gameState=GameState::END;
    this->unscheduleUpdate();//updateを止める
    updateScore(calcTimeBonus());//トタールスコアを表示
    
    auto winSize=Director::getInstance()->getWinSize();
    auto layer=Layer::create();
    
    auto label=Label::createWithSystemFont(StringUtils::format("SCORE:%d",_score),"Marker Felt",64);
    label->setColor(Color3B(234,240,38));
    label->setPosition(Vec2(winSize.width/2,winSize.height/2-100));
    label->setOpacity(0);
    layer->addChild(label);
    
    auto highScore=UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY,0);
    if(_score>highScore){//ハイスコアを更新した
        UserDefault::getInstance()->setIntegerForKey(HIGHSCORE_KEY,_score);
        auto newLabel=Label::createWithSystemFont("NEW RECORD","Marker Felt",64);
        newLabel->setColor(label->getColor());
        newLabel->setPosition(winSize/2);
        newLabel->setOpacity(0);
        newLabel->runAction(Sequence::create(DelayTime::create(1.2)
                                             ,FadeIn::create(0.3)
                                             ,NULL));
        layer->addChild(newLabel);
    }
    
    auto sprite=Sprite::create("congratulations.png");
    sprite->setScale(0);
    sprite->runAction(Sequence::create(ScaleTo::create(1,1)
                                       ,CallFunc::create([label](){
        label->runAction(FadeIn::create(0.2));
    })
                                       ,NULL));
    sprite->setPosition(Vec2(winSize.width/2,winSize.height/2+100));
    layer->addChild(sprite);
    
    //新しいゲームをはじめるボタン
    auto newButton=MenuItemImage::create("button_new_off.png"
                                         ,"button_new_on.png"
                                         ,[this](Ref* pButton){
                                             this->createCards();
                                             this->resetGame();
                                         });
    newButton->setPosition(Vec2(winSize.width/2,winSize.height/2-300));
    newButton->setOpacity(0);
    newButton->runAction(Sequence::create(DelayTime::create(1.5)
                                          ,FadeIn::create(0.5)
                                          ,NULL));
    
    auto menu=Menu::create(newButton, NULL);
    menu->setPosition(Vec2::ZERO);
    layer->addChild(menu);
    
    this->addChild(layer,convertLayerZPositionIntoInt(LayerZPosition::COVER));
    setEndLayer(layer);
}

int GameMain::calcTimeBonus()
{
    int time=_time;
    if(time<30){
        time=30;
    }
    return TIME_BONUS/time;
}

void GameMain::createMenuButton()
{
    auto winSize=Director::getInstance()->getWinSize();
    auto button=MenuItemImage::create("menu_close.png","menu_close.png",[this](Ref* pButton){
        if(_gameState!=GameState::DRAG){//ドラッグ中は使用不可
            this->createMenu();
        }
    });
    button->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    button->setPosition(Vec2(winSize.width-1,winSize.height-1));
    
    auto menu=Menu::create(button,NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu,convertLayerZPositionIntoInt(LayerZPosition::MENU));
}

void GameMain::createMenu()
{
    Director::getInstance()->pause();
    
    auto winSize=Director::getInstance()->getWinSize();
    auto layer=Layer::create();

    //プラットフォームがウィンドウズ系の場合は終了ボタンを設置しない
#if !(CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    auto sprite=Sprite::create("menuBoard_end.png");
#else
    auto sprite=Sprite::create("menuBoard.png");
#endif
    sprite->setPosition(winSize/2);
    layer->addChild(sprite);
    
    auto highScore=UserDefault::getInstance()->getIntegerForKey(HIGHSCORE_KEY,0);
    auto label=Label::createWithSystemFont(StringUtils::format("%d",highScore),"Marker Felt",48);
    label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    label->setPosition(Vec2(650,1100));
    layer->addChild(label);
    
    auto button=MenuItemImage::create("menu_open.png","menu_open.png",[layer](Ref* pButton){
        layer->removeFromParent();
        Director::getInstance()->resume();
    });
    button->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    button->setPosition(Vec2(winSize.width-1,winSize.height-1));
    
    //やり直し カードを初期配置に戻す
    auto resetButton=MenuItemImage::create("button_off.png"
                                           ,"button_on.png"
                                           ,[this](Ref* pButton){
                                               this->resetGame();
                                           });
    resetButton->setPosition(Vec2(290,700));
    
    //再配布 カードを配りなおす
    auto restartButton=MenuItemImage::create("button_off.png"
                                             ,"button_on.png"
                                             ,[this](Ref* pButton){
                                                 this->createCards();
                                                 this->resetGame();
                                             });
    restartButton->setPosition(Vec2(290,570));
    
    //helpボタン
    auto helpButton=MenuItemImage::create("button_help_off.png"
                                          ,"button_help_on.png"
                                          ,[this](Ref* pButton){
                                              this->createHelp();
                                          });
    helpButton->setPosition(Vec2(320,1200));
    
#if !(CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    //アプリ終了ボタン
    auto apEndButton=MenuItemImage::create("button_off.png",
                                           "button_on.png"
                                           ,[this](Ref* pButton){
                                               this->unscheduleUpdate();
                                               this->unschedule(DRAG_SCHEDULE);
                                               Director::getInstance()->getActionManager()->pauseAllRunningActions();
                                               Director::getInstance()->getActionManager()->removeAllActions();
                                               
                                               Director::getInstance()->end();
                                               if(CC_TARGET_PLATFORM==CC_PLATFORM_IOS){
                                                   exit(0);
                                               }
                                           });
    apEndButton->setPosition(Vec2(350,320));
    
    auto menu=Menu::create(button,resetButton,restartButton,helpButton,apEndButton,NULL);
#else
    auto menu=Menu::create(button,resetButton,restartButton,helpButton,NULL);
#endif
    menu->setPosition(Vec2::ZERO);
    layer->addChild(menu);
    
    //山札からカードを引く枚数
    auto oneButtonOff=Sprite::create("button_off.png");
    oneButtonOff->setPosition(Vec2(290,850));
    layer->addChild(oneButtonOff);
    auto oneButton=Sprite::create("button_on.png");
    oneButton->setPosition(oneButtonOff->getPosition());
    layer->addChild(oneButton);
    
    auto threeButtonOff=Sprite::create("button_off.png");
    threeButtonOff->setPosition(Vec2(460,850));
    layer->addChild(threeButtonOff);
    auto threeButton=Sprite::create("button_on.png");
    threeButton->setPosition(threeButtonOff->getPosition());
    layer->addChild(threeButton);
    
    if(_talonLayer->getPullCount()==1){//カードが1枚の場合onebuttonを点灯
        threeButton->setVisible(false);
    }else{
        oneButton->setVisible(false);
    }
    
    auto listener=EventListenerTouchOneByOne::create();
    listener->onTouchBegan=[](Touch* touch,Event* event){return true;};
    listener->onTouchEnded=[this,oneButton,threeButton](Touch* touch,Event* event){
        auto position=touch->getLocation();
        
        if(oneButton->getBoundingBox().containsPoint(position)){//onebuttonがタッチされた
            if(!oneButton->isVisible()){//onebuttonが点灯状態ではない
                UserDefault::getInstance()->setIntegerForKey(PULLCOUNT_KEY,1);
                oneButton->setVisible(true);
                threeButton->setVisible(false);
                this->resetGame();
            }
        }else if(threeButton->getBoundingBox().containsPoint(position)){
            if(!threeButton->isVisible()){
                UserDefault::getInstance()->setIntegerForKey(PULLCOUNT_KEY,3);
                oneButton->setVisible(false);
                threeButton->setVisible(true);
                this->resetGame();
            }
        }
    };
    listener->setSwallowTouches(true);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,layer);
    
    
    this->addChild(layer,convertLayerZPositionIntoInt(LayerZPosition::MENU));
}

void GameMain::resetGame()
{
    this->unscheduleUpdate();
    this->unschedule(DRAG_SCHEDULE);
    
    if(_endLayer){
        _endLayer->removeFromParent();
        setEndLayer(nullptr);
    }
    _time=0;
    _timeLabel->setString(StringUtils::format("%d",static_cast<int>(_time)));
    _score=0;
    _scoreLabel->setString(StringUtils::format("%d",_score));
    //カードがレイヤーにあるので、レイヤーを削除しカードの親を無くす
    setDragLayer(nullptr);
    _talonLayer->removeFromParent();
    setTalonLayer(nullptr);
    _boardCardLayer->removeFromParent();
    setBoardCardLayer(nullptr);
    _homeCellLayer->removeFromParent();
    setHomeCellLayer(nullptr);
    
    createBoards();
    settingStart();
}

void GameMain::dragTouchEnded(const cocos2d::Vec2 &position)
{
    _moveLayer->removeAllChildren();
    _gameState=GameState::TOUCH_WAITING;
    if(BOARD_CARD_AREA_RECT.containsPoint(position)){
        if(_boardCardLayer->dropCards(position,_moveCards,_dragLayer==_boardCardLayer)){
            //ドロップ成功
            //山札から場札へ+5
            if(_dragLayer==_talonLayer){
                this->updateScore(5);
            }
            
            //場札をめくる+5
            if(_dragLayer==_boardCardLayer){
                if(_boardCardLayer->turnCard()){
                    this->updateScore(5);
                }
            }
            
            //ホームセルから場札へ-15
            if(_dragLayer==_homeCellLayer){
                this->updateScore(-15);
            }
            return;
        }
    }else if(HOME_CELL_AREA_RECT.containsPoint(position)){
        if(_homeCellLayer->dropCards(position,_moveCards,_dragLayer==_homeCellLayer)){
            //ホームセルに移す+10
            this->updateScore(10);
            
            //場札をめくる+5
            if(_dragLayer==_boardCardLayer){
                if(_boardCardLayer->turnCard()){
                    this->updateScore(5);
                }
            }
            if(_homeCellLayer->checkAllCards()){
                this->settingEnd();
            }
            return;
        }
    }
    
    _dragLayer->cancelCards(_moveCards);
}

void GameMain::createHelp()
{
    auto layer=LayerColor::create(Color4B::BLACK);
    
    auto winSize=Director::getInstance()->getWinSize();
    
    
    Vector<Sprite*> helps;
    Vector<Sprite*> buttons;
    for(int i=1;i<=4;i++){
        //ヘルプ作成
        auto help=Sprite::create(StringUtils::format("help%d.png",i));
        help->setPosition(winSize/2);
        help->setVisible(false);
        layer->addChild(help);
        helps.pushBack(help);
        
        //ヘルプ選択ボタン作成
        auto button=Sprite::create("button_help_select_off.png");
        auto onButton=Sprite::create("button_help_select_on.png");
        onButton->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        onButton->setPosition(Vec2::ZERO);
        onButton->setVisible(false);
        button->addChild(onButton);
        button->setPosition(HELP_SELECT+HELP_SELECT_DIFF*(i-1));
        layer->addChild(button);
        buttons.pushBack(button);
    }
    helps.front()->setVisible(true);
    buttons.front()->getChildren().front()->setVisible(true);
    
    auto listener=EventListenerTouchOneByOne::create();
    listener->onTouchBegan=[](Touch* touch,Event* event){return true;};
    listener->onTouchEnded=[helps,buttons](Touch* touch,Event* event){
        auto position=touch->getLocation();
        
        for(int i=0;i<4;i++){
            auto button=buttons.at(i);
            //どのボタンがタッチされたか調べる
            if(button->getBoundingBox().containsPoint(position)){
                //タッチされたボタンに対応するヘルプを表示
                helps.at(i)->setVisible(true);
                button->getChildren().front()->setVisible(true);
                
                //他のヘルプは消す
                for(int j=0;j<4;j++){
                    if(j==i){continue;}//表示するヘルプは飛ばす
                    helps.at(j)->setVisible(false);
                    buttons.at(j)->getChildren().front()->setVisible(false);
                }
                
                break;
            }
        }
    };
    listener->setSwallowTouches(true);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,layer);
    
    auto close=MenuItemImage::create("button_close_off.png"
                                     ,"button_close_on.png"
                                     ,[layer](Ref* pButton){
                                         layer->removeFromParent();
                                     });
    close->setPosition(Vec2(320,1200));
    
    auto menu=Menu::create(close,NULL);
    menu->setPosition(Vec2::ZERO);
    layer->addChild(menu);
    
    this->addChild(layer,convertLayerZPositionIntoInt(LayerZPosition::HELP));
}