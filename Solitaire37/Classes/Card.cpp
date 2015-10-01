//
//  Card.cpp
//  Solitaire
//
//  Created by kk on 2015/08/10.
//
//

#include "Card.h"

const int CARD_WIDTH=100;
const int CARD_HEIGHT=150;

Card::Card()
{
    
}

Card::~Card()
{
    
}

Card* Card::createWithTypeANumber(Card::CardType type,int number)
{
    Card* card=new Card();
    if(card->initWithTypeANumber(type,number)){
        card->autorelease();
        return card;
    }
    
    CC_SAFE_RELEASE_NULL(card);
    return nullptr;
}

bool Card::initWithTypeANumber(Card::CardType type,int number)
{
    if(number<1 || number>13){return false;}//数字が1未満もしくは13より大きい場合はfalse
    
    string fileName;
    switch(type){
        case CardType::HEART:
            fileName="heart.png";
            break;
        
        case CardType::SPADE:
            fileName="spade.png";
            break;
            
        case CardType::DIAMOND:
            fileName="diamond.png";
            break;
            
        case CardType::CLUB:
            fileName="club.png";
            break;
            
        default:
            break;
    }
    
    if(!Sprite::initWithFile(fileName,calcCardRect(number))){
        return false;
    }
    
    _cardType=type;
    _number=number;
    
    return true;
    
}

Sprite* Card::createHeart(int number)
{
    if(number<1 || number>13){return nullptr;}
    auto sprite=Sprite::create("heart.png",calcCardRect(number));
    return sprite;
}

Sprite* Card::createSpade(int number)
{
    if(number<1 || number>13){return nullptr;}
    auto sprite=Sprite::create("spade.png",calcCardRect(number));
    return sprite;
}

Sprite* Card::createDiamond(int number)
{
    if(number<1 || number>13){return nullptr;}
    auto sprite=Sprite::create("diamond.png",calcCardRect(number));
    return sprite;
}

Sprite* Card::createClub(int number)
{
    if(number<1 || number>13){return nullptr;}
    auto sprite=Sprite::create("club.png",calcCardRect(number));
    return sprite;
}

Rect Card::calcCardRect(int number)
{
    return Rect((number-1)*CARD_WIDTH,0,CARD_WIDTH,CARD_HEIGHT);
}

int Card::convertCardTypeIntoInt(Card::CardType type)
{
    return static_cast<int>(type);
}