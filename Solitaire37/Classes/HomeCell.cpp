//
//  HomeCell.cpp
//  Solitaire
//
//  Created by kk on 2015/08/28.
//
//

#include "HomeCell.h"

const Vec2 TALON_POSITION=Vec2(360,1050);
const Vec2 TALON_LAYER_DIFF=Vec2(102,0);

//HomeCellTalon
HomeCell::HomeCellTalon::HomeCellTalon()
:_cardFrame(nullptr)
{
    
}

HomeCell::HomeCellTalon::~HomeCellTalon()
{
    CC_SAFE_RELEASE_NULL(_cardFrame);
}

HomeCell::HomeCellTalon* HomeCell::HomeCellTalon::createWithCardType(Card::CardType cardType)
{
    auto ret=new HomeCellTalon::HomeCellTalon();
    if(ret->initWithCardType(cardType)){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE_NULL(ret);
    return nullptr;
}

bool HomeCell::HomeCellTalon::initWithCardType(Card::CardType cardType)
{
    if(!Layer::init()){return false;}
    
    _cardType=cardType;
    
    //枠作成
    string fileName;
    switch(cardType){
        case Card::CardType::HEART:
            fileName="heart_frame.png";
            break;
            
        case Card::CardType::SPADE:
            fileName="spade_frame.png";
            break;
            
        case Card::CardType::DIAMOND:
            fileName="diamond_frame.png";
            break;
            
        case Card::CardType::CLUB:
            fileName="club_frame.png";
            break;
            
        default:
            break;
    }
    
    auto cardFrame=Sprite::create(fileName);
    cardFrame->setPosition(TALON_POSITION);
    this->addChild(cardFrame,0);
    setCardFrame(cardFrame);
    
    return true;
}

Vector<Card*> HomeCell::HomeCellTalon::dragCards(const cocos2d::Vec2 &position)
{
    auto localPosition=convertToNodeSpace(position);
    
    Vector<Card*> cards;
    if(!_cards.empty()){//組札がある
        if(_cardFrame->getBoundingBox().containsPoint(localPosition)){//組札をタッチ
            auto card=_cards.back();
            cards.pushBack(card);
            card->removeFromParent();
            _cards.popBack();
        }
    }
    
    return move(cards);
}

void HomeCell::HomeCellTalon::cancelCards(Vector<Card *> &cards)
{
    auto card=cards.back();
    Sprite* topCard;
    if(_cards.empty()){
        topCard=_cardFrame;
    }else{
        topCard=_cards.back();
    }
    _cards.pushBack(card);
    cards.clear();
    
    card->setPosition(TALON_POSITION);
    this->addChild(card,topCard->getLocalZOrder());
}

bool HomeCell::HomeCellTalon::dropCards(const cocos2d::Vec2 &position,Vector<Card *> &cards)
{
    auto localPosition=convertToNodeSpace(position);
    if(!_cardFrame->getBoundingBox().containsPoint(localPosition)){return false;}//枠をタッチしていない
    
    auto card=cards.back();
    if(card->getCardType()!=_cardType){return false;}//カードの絵柄が違う
    if(_cards.empty()){//組札が無い
        if(card->getNumber()!=1){return false;}//組札が無いとき置けるのはAのみ
        this->addChild(card,_cardFrame->getLocalZOrder()+1);
    }else{
        auto topCard=_cards.back();
        if(card->getNumber()-topCard->getNumber()!=1){return false;}//上の組み札との差は1
        this->addChild(card,topCard->getLocalZOrder()+1);
    }
    
    card->setPosition(TALON_POSITION);
    _cards.pushBack(card);
    cards.clear();
    
    return true;
}

bool HomeCell::HomeCellTalon::checkAllCards()
{
    return (_cards.size()==13);
}

bool HomeCell::HomeCellTalon::checkDropCard(Vector<Card *> &cards)
{
    auto card=cards.back();
    if(card->getCardType()!=_cardType){return false;}//カードの絵柄が違う
    if(_cards.empty()){//組札がない
        if(card->getNumber()==1){return true;}//カードがA
    }else{
        auto topCard=_cards.back();
        if(card->getNumber()-topCard->getNumber()==1){return true;}//組み札との差が1
    }
    
    return false;
}
//e HomeCellTalon

//HomeCell
HomeCell::HomeCell()
:_dragLayer(nullptr)
{
    
}

HomeCell::~HomeCell()
{
    CC_SAFE_RELEASE_NULL(_dragLayer);
}

HomeCell* HomeCell::create()
{
    auto ret=new HomeCell::HomeCell();
    if(ret->init()){
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE_NULL(ret);
    return nullptr;
}

bool HomeCell::init()
{
    if(!Layer::init()){return false;}
    
    //組札作成
    auto heartTalon=HomeCellTalon::createWithCardType(Card::CardType::HEART);
    _talons.pushBack(heartTalon);
    this->addChild(heartTalon);
    
    auto spadeTalon=HomeCellTalon::createWithCardType(Card::CardType::SPADE);
    _talons.pushBack(spadeTalon);
    spadeTalon->setPosition(TALON_LAYER_DIFF);
    this->addChild(spadeTalon);
    
    auto diamondTalon=HomeCellTalon::createWithCardType(Card::CardType::DIAMOND);
    _talons.pushBack(diamondTalon);
    diamondTalon->setPosition(TALON_LAYER_DIFF*2);
    this->addChild(diamondTalon);
    
    auto clubTalon=HomeCellTalon::createWithCardType(Card::CardType::CLUB);
    _talons.pushBack(clubTalon);
    clubTalon->setPosition(TALON_LAYER_DIFF*3);
    this->addChild(clubTalon);
    
    return true;
}

Vector<Card*> HomeCell::dragCards(const cocos2d::Vec2 &position)
{
    Vector<Card*> cards;
    
    for(auto talon:_talons){
        cards=talon->dragCards(position);
        if(!cards.empty()){
            setDragLayer(talon);
            break;
        }
    }
    
    return cards;
}

void HomeCell::cancelCards(Vector<Card *> &cards)
{
    if(cards.empty()){return;}
    
    if(_dragLayer){
        _dragLayer->cancelCards(cards);
    }
}

bool HomeCell::dropCards(const cocos2d::Vec2 &position,Vector<Card *> &cards,bool isDrag)
{
    if(cards.size()!=1){return false;}
    for(auto talon:_talons){
        if(isDrag && talon==_dragLayer){continue;}//このレイヤーからドラッグされた場合は飛ばす
        if(talon->dropCards(position,cards)){
            return true;
        }
    }
    
    return false;
}

bool HomeCell::checkAllCards()
{
    for(auto talon:_talons){
        if(!talon->checkAllCards()){
            return false;
        }
    }
    
    return true;
}

bool HomeCell::checkDropCard(Vector<Card *> &cards)
{
    if(cards.size()!=1){return false;}//カードが1枚ではない
    
    for(auto talon:_talons){
        if(talon->checkDropCard(cards)){//カードを置ける
            return true;
        }
    }
    
    return false;
}

Vec2 HomeCell::getHomeCellTalonPosition(Card::CardType cardType)
{
    auto position=Vec2(0,0);
    for(auto talon:_talons){
        if(cardType==talon->getCardType()){
            position=talon->getPosition()+TALON_POSITION;
            break;
        }
    }
    
    return position;
}
//e HomeCell