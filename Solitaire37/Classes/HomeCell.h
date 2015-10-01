//
//  HomeCell.h
//  Solitaire
//
//  Created by kk on 2015/08/28.
//
//

#ifndef __Solitaire__HomeCell__
#define __Solitaire__HomeCell__

#include <stdio.h>
#include "cocos2d.h"
#include "Card.h"
#include "BaseLayer.h"

USING_NS_CC;
using namespace std;

//ホームセルの管理クラス
class HomeCell:public BaseLayer
{
protected:
    //カード置き場
    class HomeCellTalon:public BaseLayer
    {
    protected:
        HomeCellTalon();
        virtual ~HomeCellTalon();
        bool initWithCardType(Card::CardType cardType);
        
    public:
        CC_SYNTHESIZE(Card::CardType,_cardType,CardType);//置けるカードの種類
        CC_SYNTHESIZE(Vector<Card*>,_cards,Cards);//置かれたカード
        CC_SYNTHESIZE_RETAIN(Sprite*,_cardFrame,CardFrame);//枠
        
        static HomeCellTalon* createWithCardType(Card::CardType cardType);
        
        Vector<Card*> dragCards(const Vec2& position) override;
        void cancelCards(Vector<Card*>& cards) override;
        
        bool dropCards(const Vec2& position,Vector<Card*>& cards);
        
        bool checkAllCards();
        
        bool checkDropCard(Vector<Card*>& cards);
    };
    
    HomeCell();
    virtual ~HomeCell();
    bool init() override;
public:
    CC_SYNTHESIZE(Vector<HomeCellTalon*>,_talons,Talons);//組札
    CC_SYNTHESIZE_RETAIN(HomeCellTalon*,_dragLayer,DragLayer);//ドラッグされたレイヤー
    
    static HomeCell* create();
    
    Vector<Card*> dragCards(const Vec2& position) override;
    void cancelCards(Vector<Card*>& cards) override;
    
    //カードをドロップ
    //@param position タッチした座標
    //@param cards ドロップするカード
    //@param isDrag このレイヤーからドラッグされたか
    //@return true:ドロップ成功
    bool dropCards(const Vec2& position,Vector<Card*>& cards,bool isDrag);
    
    //全てのカードがホームセルにあるか
    //@return true:ある
    bool checkAllCards();
    
    //ホームセルにカードをドロップできるか調べる
    //@param cards ドロップするカード 1枚のみ
    //@return true:ドロップ可能
    bool checkDropCard(Vector<Card*>& cards);
    //指定した絵柄のホームセルの位置を取得する
    //@param cardType　カードの絵柄
    //@preturn 絵柄のホームセルの座標
    Vec2 getHomeCellTalonPosition(Card::CardType cardType);
};

#endif /* defined(__Solitaire__HomeCell__) */