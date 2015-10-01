//
//  Card.h
//  Solitaire
//
//  Created by kk on 2015/08/10.
//
//

#ifndef __Solitaire__Card__
#define __Solitaire__Card__

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class Card:public Sprite
{
public:
    //カードの絵柄
    enum class CardType{
        HEART=1,
        SPADE=2,
        DIAMOND=3,
        CLUB=4
    };
    
    CC_SYNTHESIZE_READONLY(CardType,_cardType,CardType);//このカードの絵柄
    CC_SYNTHESIZE_READONLY(int,_number,Number);//このカードの数字
    
    //カードを作成する
    //@param type カードの絵柄
    //@param number カードの数字
    //@return 作成したカード
    static Card* createWithTypeANumber(CardType type,int number);
    
    //ハートのカードを返す
    //@param number カードの数字
    //@return カードのsprite
    static Sprite* createHeart(int number);
    
    //スペードのカードを返す
    //@param number カードの数字
    //@return カードのsprite
    static Sprite* createSpade(int number);
    
    //ダイアのカードを返す
    //@param number カードの数字
    //@return カードのsprite
    static Sprite* createDiamond(int number);
    
    //clubのカードを返す
    //@param number カードの数字
    //@return カードのsprite
    static Sprite* createClub(int number);
    
    //cardtypeをintに変換
    //@param type　変換するcardtype
    //@return 変換した整数
    static int convertCardTypeIntoInt(CardType type);
    
protected:
    Card();
    virtual ~Card();
    bool initWithTypeANumber(CardType type,int number);
    
    //カードのrectを計算する 画像からの切り抜き用
    //@param number カードの数字
    //@return Rect 切り取り範囲
    static Rect calcCardRect(int number);
};

#endif /* defined(__Solitaire__Card__) */
