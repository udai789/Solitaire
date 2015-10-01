//
//  BoardCard.cpp
//  Solitaire
//
//  Created by kk on 2015/08/18.
//
//

#include "BoardCard.h"

const auto TALON_POSITION=Vec2(54,890);
const auto CLOSE_TALON_DIFF=Vec2(0,-10);
const auto OPEN_TALON_DIFF=Vec2(0,-60);
const auto TALON_LAYER_DIFF=Vec2(102,0);
const auto CARD_SIZE=Vec2(100,150);

////BoardTalon
BoardCard::BoardTalon::BoardTalon()
:_cardFrame(nullptr)
{
    
}

BoardCard::BoardTalon::~BoardTalon()
{
    CC_SAFE_RELEASE_NULL(_cardFrame);
}

BoardCard::BoardTalon* BoardCard::BoardTalon::createWithCards(Vector<Card *> &cards)
{
    auto ret=new BoardTalon::BoardTalon();
    if(ret->initWithCards(cards)){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE_NULL(ret);
    return nullptr;
}

bool BoardCard::BoardTalon::initWithCards(Vector<Card *> &cards)
{
    if(!Layer::init()){return false;}
    
    auto card=cards.back();
    _openCards.pushBack(card);
    cards.popBack();
    
    _closeCards=move(cards);
    for(auto card:_closeCards){//裏向きのカードに、カードの背面を置く
        auto sprite=Sprite::create("card_back.png");
        sprite->setPosition(CARD_SIZE/2);
        card->addChild(sprite);
    }
    
    //枠を設置
    auto sprite=Sprite::create("card_frame.png");
    sprite->setPosition(TALON_POSITION);
    this->addChild(sprite,0);
    setCardFrame(sprite);
    
    showTalon();
    
    return true;
}

void BoardCard::BoardTalon::showTalon()
{
    auto position=TALON_POSITION;
    int z=_cardFrame->getLocalZOrder()+1;
    for(auto card:_closeCards){
        card->setPosition(position);
        this->addChild(card,z);
        z++;
        position+=CLOSE_TALON_DIFF;
    }
    for(auto card:_openCards){
        card->setPosition(position);
        this->addChild(card,z);
        z++;
        position+=OPEN_TALON_DIFF;
    }
}

void BoardCard::BoardTalon::hideTalon()
{
    this->removeAllChildren();
    this->addChild(_cardFrame);
}

void BoardCard::BoardTalon::dropCardFrame(Vector<Card *> cards)
{
    cards.reverse();//表示の上をスタックの下にする
    auto position=_cardFrame->getPosition();
    auto z=_cardFrame->getLocalZOrder()+1;
    
    while(!cards.empty()){
        auto card=cards.back();
        _openCards.pushBack(card);
        cards.popBack();
        
        card->setPosition(position);
        this->addChild(card,z);
        position+=OPEN_TALON_DIFF;
        z++;
    }
}

void BoardCard::BoardTalon::dropCloseCards(Vector<Card *> cards)
{
    cards.reverse();//表示の上をスタックの下にする
    auto position=_closeCards.back()->getPosition()+CLOSE_TALON_DIFF;
    auto z=_closeCards.back()->getLocalZOrder()+1;
    
    while(!cards.empty()){
        auto card=cards.back();
        _openCards.pushBack(card);
        cards.popBack();
        
        card->setPosition(position);
        this->addChild(card,z);
        position+=OPEN_TALON_DIFF;
        z++;
    }
}

void BoardCard::BoardTalon::dropOpenCards(Vector<Card *> cards)
{
    cards.reverse();//表示の上をスタックの下にする
    auto position=_openCards.back()->getPosition()+OPEN_TALON_DIFF;
    auto z=_openCards.back()->getLocalZOrder()+1;
    
    while(!cards.empty()){
        auto card=cards.back();
        _openCards.pushBack(card);
        cards.popBack();
        
        card->setPosition(position);
        this->addChild(card,z);
        position+=OPEN_TALON_DIFF;
        z++;
    }
}

Vector<Card*> BoardCard::BoardTalon::dragCards(const cocos2d::Vec2 &position)
{
    Vector<Card*> cards;
    auto localPosition=convertToNodeSpace(position);//このレイヤーの座標系に変換
    
    if(!_openCards.empty()){
        int count=static_cast<int>(_openCards.size());//表のカードの枚数
        int index=-1;//タッチされたカードの位置
        for(int i=count-1;i>=0;i--){//スタックの上のカードから調べる
            if(_openCards.at(i)->getBoundingBox().containsPoint(localPosition)){
                index=i;
                break;
            }
        }
        if(index>=0){
            index=count-index;
            while(index){
                auto card=_openCards.back();
                cards.pushBack(card);
                _openCards.popBack();
                card->removeFromParent();
                index--;
            }
        }
    }
    
    return move(cards);
}

bool BoardCard::BoardTalon::dropCards(const cocos2d::Vec2 &postion,Vector<Card *> &cards)
{
    auto localPosition=convertToNodeSpace(postion);
    
    if(_openCards.empty()){//表のカードが無い場合
        if(_cardFrame->getBoundingBox().containsPoint(localPosition)){//枠をタッチ
            auto bottomCard=cards.front();
            if(bottomCard->getNumber()==13){//枠に置けるのはキングのみ
                dropCardFrame(cards);
                return true;
            }
        }
    }else{
        auto topCard=_openCards.back();
        if(topCard->getBoundingBox().containsPoint(localPosition)){//一番上のカードをタッチ
            auto bottomCard=cards.front();
            if(topCard->getNumber()-bottomCard->getNumber()==1){//置き場所のカードとの数値差が1
                //heart1 diamond3 spade2 club4
                auto topType=Card::convertCardTypeIntoInt(topCard->getCardType());
                auto bottomType=Card::convertCardTypeIntoInt(bottomCard->getCardType());
                auto value=topType+bottomType;
                if(value%2==1){//赤と黒の合計は奇数
                    dropOpenCards(cards);
                    return true;
                }
            }
        }
    }
    
    return false;
}

void BoardCard::BoardTalon::cancelCards(Vector<Card*>& cards)
{
    if(!_openCards.empty()){//表のカードがある
        dropOpenCards(cards);
    }else if(!_closeCards.empty()){//裏面のカードがある
        dropCloseCards(cards);
    }else{//場札が無い
        dropCardFrame(cards);
    }
}

bool BoardCard::BoardTalon::turnCard()
{
    if(_openCards.empty()){
        if(!_closeCards.empty()){
            auto card=_closeCards.back();
            _openCards.pushBack(card);
            _closeCards.popBack();
            
            card->removeAllChildren();
            
            return true;
        }
    }
    
    return false;
}
////BoardTalon

////BoardCard
BoardCard::BoardCard()
:_memoryBoardTalon(nullptr)
{
    
}

BoardCard::~BoardCard()
{
    CC_SAFE_RELEASE_NULL(_memoryBoardTalon);
}

BoardCard* BoardCard::createWithCards(Vector<Card *> &cards)
{
    auto ret=new BoardCard::BoardCard();
    if(ret->initWithCards(cards)){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE_NULL(ret);
    return nullptr;
}

bool BoardCard::initWithCards(Vector<Card *> &cards)
{
    if(!Layer::init()){return false;}
    
    if(cards.size()!=28){return false;}//場札は最初28枚
    
    auto diff=Vec2(0,0);
    for(int i=1;i<=7;i++){//場札を作成
        Vector<Card*> cs;
        for(int j=1;j<=i;j++){
            auto card=cards.back();
            cs.pushBack(card);
            cards.popBack();
        }
        auto boardTalon=BoardTalon::createWithCards(cs);
        boardTalon->setPosition(diff);
        this->addChild(boardTalon);
        _boardTalons.pushBack(boardTalon);
        diff+=TALON_LAYER_DIFF;//場札のレイヤーをずらす
    }
    
    return true;
}

Vector<Card*> BoardCard::dragCards(const cocos2d::Vec2 &position)
{
    Vector<Card*> cards;
    for(auto talon:_boardTalons){
        cards=talon->dragCards(position);
        if(!cards.empty()){//場札からドラッグできた場合
            setMemoryBoardTalon(talon);
            break;
        }
    }
    
    cards.reverse();//表示の上にあったカードをスタックの上に置く
    return move(cards);
}

bool BoardCard::dropCards(const cocos2d::Vec2 &position,Vector<Card *> &cards,bool isDrag)
{
    if(cards.empty()){return false;}
    
    for(auto talon:_boardTalons){
        if(isDrag && talon==_memoryBoardTalon){continue;}//ドラッグ元の場札は飛ばす
        if(talon->dropCards(position,cards)){
            return true;//ドラッグできればtrueを返し終了
        }
    }
    
    return false;
}

void BoardCard::cancelCards(Vector<Card*>& cards)
{
    if(cards.empty()){return;}
    if(_memoryBoardTalon){
        _memoryBoardTalon->cancelCards(cards);
    }
}

bool BoardCard::turnCard()
{
    for(auto talon:_boardTalons){
        if(talon->turnCard()){
            return true;
        }
    }
    
    return false;
}
////BoardCard