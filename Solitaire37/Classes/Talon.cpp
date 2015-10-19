//
//  Talon.cpp
//  Solitaire
//
//  Created by kk on 2015/08/16.
//
//

#include "Talon.h"

const auto TALON_CARD_POSITION=Vec2(54,1050);//山札の位置
const auto SHOW_CARD_POSITION1=Vec2(170,1050);//表示カードの位置
const auto SHOW_CARD_POSITION2=Vec2(200,1050);
const auto SHOW_CARD_POSITION3=Vec2(230,1050);

const int CARD_WIDTH=100;//カードの幅

Talon::Talon()
:_pullCount(3)
,_dragCardPosition(Vec2(0,0))
,_talonCardSprite(nullptr)
,_talonCardFrame(nullptr)
{
    
}

Talon::~Talon()
{
    CC_SAFE_RELEASE_NULL(_talonCardSprite);
    CC_SAFE_RELEASE_NULL(_talonCardFrame);
    for(auto card:_talonCards){
        card->setVisible(true);//非表示にしていたものを表示する
    }
    for(auto card:_pullCards){
        card->stopAllActions();
    }
}

Talon* Talon::createWithCardsAPullCount(Vector<Card *> &cards,int pullCount)
{
    auto ret=new Talon::Talon();
    if(ret->initWithCardsAPullCount(cards,pullCount)){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE_NULL(ret);
    return nullptr;
}

bool Talon::initWithCardsAPullCount(Vector<Card *> &cards,int pullCount)
{
    if(!Layer::init()){return false;}
    if(pullCount!=1 && pullCount!=3){return false;}//1でもなく3でもない
    _pullCount=pullCount;
    
    _showCardsPosition.push_back(SHOW_CARD_POSITION1);
    _showCardsPosition.push_back(SHOW_CARD_POSITION2);
    _showCardsPosition.push_back(SHOW_CARD_POSITION3);
    
    createTalonCardSprite();
    
    _talonCards=move(cards);
    //レイヤーに設置し消しておく
    for(auto card:_talonCards){
        this->addChild(card);
        card->setVisible(false);
    }
    
    return true;
}

void Talon::moveTalonToPull()
{
    //前に引いたカードが残っている場合の処理
    if(!_pullCards.empty()){//置き札がある
        int index=static_cast<int>(_pullCards.size())-1;
        for(int i=index;i>=0;i--){
            if(_pullCards.at(i)->getPosition()==_showCardsPosition.at(0)){break;}//置き札の基準位置の場合終了
            _pullCards.at(i)->setPosition(_showCardsPosition.at(0));//置き札の基準位置に設置
        }
    }
    
    //カードを引く処理
    for(int i=0;i<_pullCount;i++){//山札から引く回数分引く
        if(_talonCards.empty()){break;}//山札にカードがない
        auto card=_talonCards.back();
        card->setVisible(true);
        
        if(_pullCards.empty()){//置き札がない
            card->setLocalZOrder(0);
        }else{
            card->setLocalZOrder(_pullCards.back()->getLocalZOrder()+1);
        }
        //card->setPosition(_showCardsPosition.at(i));
        ////
        card->setPosition(TALON_CARD_POSITION+Vec2(CARD_WIDTH,0));
        card->runAction(MoveTo::create(0.2,_showCardsPosition.at(i)));
        ////
        
        
        _pullCards.pushBack(card);
        _talonCards.popBack();
    }
    
    if(_talonCards.empty()){//山札のカードが無くなったので非表示に
        _talonCardSprite->setVisible(false);
    }
}

bool Talon::movePullToTalon()
{
    if(_pullCards.empty()){return false;}//置き札がない
    for(auto card:_pullCards){//カードの表示を消す
        card->setVisible(false);
    }
    _pullCards.reverse();//底にあるカードが山札の上
    _talonCards=move(_pullCards);
    _talonCardSprite->setVisible(true);
    
    return true;
}

bool Talon::checkTouchTalon(const cocos2d::Vec2 &position)
{
    if(!_talonCardFrame){return false;}
    return _talonCardFrame->getBoundingBox().containsPoint(position);
}

bool Talon::nextShowCards()
{
    bool result=false;
    
    if(!_talonCards.empty()){//山札がある
        moveTalonToPull();
    }else{//山札がない
        result=movePullToTalon();
    }
    
    return result;
}

bool Talon::touchTalonCard(const cocos2d::Vec2 &position)
{
    ////
    stopCardsActions();
    ////
    if(checkTouchTalon(position)){
        return nextShowCards();
    }
    
    return false;
}

Vector<Card*> Talon::dragCards(const cocos2d::Vec2 &position)
{
    ////
    stopCardsActions();
    ////
    Vector<Card*> cards;
    if(!_pullCards.empty()){//置き札がある
        auto card=_pullCards.back();
        
        if(card->getBoundingBox().containsPoint(position)){
            cards.pushBack(card);
            _dragCardPosition=card->getPosition();
            card->removeFromParent();
            _pullCards.popBack();
        }
    }
    
    return move(cards);
}

void Talon::cancelCards(Vector<Card *> &cards)
{
    if(cards.empty()){return;}
    auto card=cards.back();
    card->setPosition(_dragCardPosition);
    this->addChild(card);
    if(_pullCards.empty()){
        card->setLocalZOrder(0);
    }else{
        card->setLocalZOrder(_pullCards.back()->getLocalZOrder()+1);
    }
    _pullCards.pushBack(card);
    cards.clear();
}

void Talon::createTalonCardSprite()
{
    //表示枠の作成
    auto frame=Sprite::create("card_frame.png");
    frame->setPosition(TALON_CARD_POSITION);
    this->addChild(frame);
    setTalonCardFrame(frame);
    
    //山札を表すカードの作成
    auto sprite=Sprite::create("card_back.png");
    sprite->setPosition(TALON_CARD_POSITION);
    this->addChild(sprite);
    setTalonCardSprite(sprite);
}

void Talon::stopCardsActions()
{
    if(!_pullCards.empty()){
        if(_pullCards.back()->getNumberOfRunningActions()){//アクション中
            int i=0;
            auto index=_pullCards.size()-1;//表示カードの最後の添字
            if(index==1){//カードが2枚
                if(_pullCards.at(index-1)->getNumberOfRunningActions()){
                    i=1;
                }
            }else if(index>=2){//カードが3枚以上
                if(_pullCards.at(index-2)->getNumberOfRunningActions()){
                    i=2;
                }
            }
            
            int j=0;
            while(i>=0){//アクション中のカードを停止し、指定された位置に置く
                auto card=_pullCards.at(index-i);
                card->stopAllActions();
                card->setPosition(_showCardsPosition.at(j));
                
                i--;
                j++;
            }
        }
    }
}