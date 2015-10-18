//
//  Talon.h
//  Solitaire
//
//  Created by kk on 2015/08/16.
//
//

#ifndef __Solitaire__Talon__
#define __Solitaire__Talon__

#include <stdio.h>
#include "cocos2d.h"
#include "Card.h"
#include "BaseLayer.h"

USING_NS_CC;
using namespace std;

class Talon:public BaseLayer
{
protected:
    Talon();
    virtual ~Talon();
    bool initWithCardsAPullCount(Vector<Card*>& cards,int pullCount);
    
    CC_SYNTHESIZE(Vector<Card*>,_talonCards,TalonCards);//山札
    CC_SYNTHESIZE(Vector<Card*>,_pullCards,PullCards);//山札から引いたカード
    CC_SYNTHESIZE(Vec2,_dragCardPosition,DragCardPosition);//ドラッグしたカードの位置
    CC_SYNTHESIZE_READONLY(vector<Vec2>,_showCardsPosition,ShowCardsPosition);//山札の表示するカードの位置
    CC_SYNTHESIZE_RETAIN(Sprite*,_talonCardSprite,TalonCardSprite);//山札を表すカード
    CC_SYNTHESIZE_RETAIN(Sprite*,_talonCardFrame,TalonCardFrame);//枠
    
    void moveTalonToPull();//山札から置き札にカードを移す
    bool movePullToTalon();//置き札から山札にカードを移す @return true:置き札から山札にカードを移した
    
    void createTalonCardSprite();//山札を表すカードを作成する
    
    void stopCardsActions();//カードのアクションを停止し、カードを定位置に置く
    
public:
    
    CC_SYNTHESIZE_READONLY(int,_pullCount,PullCount);//山札から引くカードの枚数 初期設定3
    
    //山札レイヤーを作成する
    //@param cards 山札 *:引数のvectorの要素は空となる
    //@param pullCount 山札からカードを引く枚数 1or3
    //@return 山札レイヤー
    static Talon* createWithCardsAPullCount(Vector<Card*>& cards,int pullCount);
    
    
    //山札をクリックしたか
    //@param position クリックした位置
    //@return 山札上ならばtrue
    bool checkTouchTalon(const Vec2& position);
    //次の表示カードを表示する
    //@return true:山札が一巡した
    bool nextShowCards();
    //山札をタッチした場合、次の表示カードを表示する
    //@param position タッチした位置
    //@return true:山札が一巡した
    bool touchTalonCard(const Vec2& position);
    
    //カードをドラッグ
    //@param position タッチした位置
    //@return 表示の一番上のカード(このカード以外をタッチした場合はempty)
    Vector<Card*> dragCards(const Vec2& position) override;
    void cancelCards(Vector<Card*>& cards) override;//ドロップをキャンセル
};

#endif /* defined(__Solitaire__Talon__) */
