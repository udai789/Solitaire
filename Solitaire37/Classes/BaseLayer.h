//
//  BaseLayer.h
//  Solitaire
//
//  Created by kk on 2015/08/24.
//
//

#ifndef __Solitaire__BaseLayer__
#define __Solitaire__BaseLayer__

#include <stdio.h>
#include "cocos2d.h"
#include "Card.h"

USING_NS_CC;
using namespace std;

//山札、場札、ホームセルに共通の関数を実装
class BaseLayer:public Layer
{
public:
    //カードをドラッグ
    //@param position タッチした座標
    //@return タッチした座標のカード(場札はその上も)　カードが無い・動かせなければempty
    virtual Vector<Card*> dragCards(const Vec2& position)=0;
    
    //ドロップをキャンセル
    //@param cards ドラッグしたカード
    virtual void cancelCards(Vector<Card*>& cards)=0;
};

#endif /* defined(__Solitaire__BaseLayer__) */
