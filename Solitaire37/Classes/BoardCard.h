//
//  BoardCard.h
//  Solitaire
//
//  Created by kk on 2015/08/18.
//
//

#ifndef __Solitaire__BoardCard__
#define __Solitaire__BoardCard__

#include <stdio.h>
#include "cocos2d.h"
#include "Card.h"
#include "BaseLayer.h"

USING_NS_CC;
using namespace std;

//場札の管理クラス
class BoardCard:public BaseLayer
{
protected:
    //場札を表すクラス
    class BoardTalon:public BaseLayer
    {
    protected:
        BoardTalon();
        virtual ~BoardTalon();
        bool initWithCards(Vector<Card*>& cards);
        
        void showTalon();//場札を表示する レイヤーに追加
        void hideTalon();//場札を消す レイヤーから削除
        
        //枠にカードをドロップ
        //@param cards ドロップするカード
        void dropCardFrame(Vector<Card*> cards);
        
        //裏返しの場札にドロップ
        //@param cards ドロップするカード
        void dropCloseCards(Vector<Card*> cards);
        
        //表の場札にドロップ
        //@param cards ドロップするカード
        void dropOpenCards(Vector<Card*> cards);
        
    public:
        CC_SYNTHESIZE(Vector<Card*>,_closeCards,CloseCards);//裏返しの場札
        CC_SYNTHESIZE(Vector<Card*>,_openCards,OpenCards);//表の場札
        CC_SYNTHESIZE_RETAIN(Sprite*,_cardFrame,CardFrame);//カード置き場
        
        //場札を作成
        //@param cards 場札のカード
        //@return 場札
        static BoardTalon* createWithCards(Vector<Card*>& cards);
        
        Vector<Card*> dragCards(const Vec2& position) override;//カードをドラッグ
        //カードをドロップ失敗した場合はfalseを返す
        //@param position　タッチした座標
        //@param cards ドロップするカード
        //@return true:ドロップに成功 false:失敗
        bool dropCards(const Vec2& postion,Vector<Card*>& cards);
        void cancelCards(Vector<Card*>& cards) override;//ドロップをキャンセル
        
        //表の場札が無く、裏返しの場札があればめくる
        //@return true:めくった
        bool turnCard();
    };
    
    BoardCard();
    virtual ~BoardCard();
    bool initWithCards(Vector<Card*>& cards);
    
public:
    CC_SYNTHESIZE(Vector<BoardTalon*>,_boardTalons,BoardTalons);//場札の山
    CC_SYNTHESIZE_RETAIN(BoardTalon*,_memoryBoardTalon,MemoryBoardTalon);//バックアップを取る場札の記録
    
    //場札の管理クラスを作成
    //@param cards 場札のカード 28枚 このvectorの要素は空になる
    //@return 場札の管理クラス カードの枚数が28枚でなければnullptr
    static BoardCard* createWithCards(Vector<Card*>& cards);
    
    Vector<Card*> dragCards(const Vec2& position) override;//カードをドラッグ
    //カードをドロップ 失敗した場合はfalseを返す
    //@param position タッチした座標
    //@param cards ドロップするカード
    //@param isDrag このレイヤーからドラッグされたか　true:された false:違うレイヤーから
    //@return true:ドロップ成功
    bool dropCards(const Vec2& position,Vector<Card*>& cards,bool isDrag);
    void cancelCards(Vector<Card*>& cards) override;//ドロップをキャンセル
    
    //裏面のカードをめくる。このレイヤーからのドロップ成功時に呼び出す
    //@return めくった
    bool turnCard();
};

#endif /* defined(__Solitaire__BoardCard__) */
