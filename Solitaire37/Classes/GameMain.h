//
//  GameMain.h
//  Solitaire
//
//  Created by kk on 2015/08/10.
//
//

#ifndef __Solitaire__GameMain__
#define __Solitaire__GameMain__

#include <stdio.h>
#include <random>
#include "Card.h"
#include "BaseLayer.h"
#include "Talon.h"
#include "BoardCard.h"
#include "HomeCell.h"

USING_NS_CC;
using namespace std;

class GameMain:public Layer
{
protected:
    enum class LayerZPosition{
        BACKGROUND,
        BOARDS,
        MOVE,
        COVER,
        MENU,
        HELP
    };
    
    enum class GameState{
        DRAG,//ドラッグ中
        TOUCH_WAITING,//操作待機
        START,//ゲーム開始時
        END,//ゲーム終了時
        NONE
    };
    
    CC_SYNTHESIZE(GameState,_gameState,GameState);//ゲームの状態
    CC_SYNTHESIZE(Vector<Card*>,_cards,Cards);//カード
    CC_SYNTHESIZE(Vector<Card*>,_moveCards,MoveCards);//移動レイヤーのカード
    CC_SYNTHESIZE(mt19937,_engine,Engine);
    CC_SYNTHESIZE(float,_time,Time);//時間 秒
    CC_SYNTHESIZE(int,_score,Score);//スコア 0以上
    CC_SYNTHESIZE(float,_tenCount,TenCount);//10秒計測用
    CC_SYNTHESIZE_RETAIN(Layer*,_moveLayer,MoveLayer);//移動用
    CC_SYNTHESIZE_RETAIN(BaseLayer*,_dragLayer,DragLayer);//ドラッグ元のレイヤー
    CC_SYNTHESIZE_RETAIN(Talon*,_talonLayer,TalonLayer);//山札
    CC_SYNTHESIZE_RETAIN(BoardCard*,_boardCardLayer,BoardCardLayer);//場札
    CC_SYNTHESIZE_RETAIN(HomeCell*,_homeCellLayer,HomeCellLayer);;//組札
    CC_SYNTHESIZE_RETAIN(Label*,_timeLabel,TimeLabel);//時間表示
    CC_SYNTHESIZE_RETAIN(Label*,_scoreLabel,ScoreLabel);//スコア表示
    CC_SYNTHESIZE_RETAIN(Layer*,_endLayer,EndLayer);//ゲーム終了時の表示用
    
    CC_SYNTHESIZE(float,_touchTime,TouchTime);//ダブルタッチ判定、最初のbeginから2回目のendedまでの時間
    CC_SYNTHESIZE(bool,_doubleTouchFlag,DoubleTouchFlag);//ダブルタッチ判定、ダブルタッチ処理を行う可能性がある場合にtrueに
    CC_SYNTHESIZE(bool,_oneTouch,OneTouch);//ダブルタッチ判定、doublTouchFlagがtrue時に一度ended処理がされたか
    
    GameMain();
    virtual ~GameMain();
    bool init() override;
    
    int generateRandomInt(int min,int max);//min<=x<=maxの整数を作成
    
    void createBackground();//背景を作成
    void createCards();//カードを作成
    void createBoards();//レイヤーを作成を作成
    void createLabel();//ラベルを作成
    void createMenu();//メニューを作成
    void createMenuButton();//メニューボタンを作成
    void createHelp();//ヘルプを作成
    
    void shuffleCards();//カードをシャッフル
    
    void settingStart();
    void settingEnd();
    
    void update(float dt) override;
    
    //scoreに加算し、ラベルの値を更新する
    //@param value 加算する整数
    void updateScore(int value);
    int calcTimeBonus();//ボーナススコアを計算する
    
    void resetGame();//ゲームをリセット
    
    void dragTouchEnded(const Vec2& position);
public:
    CREATE_FUNC(GameMain);
    static Scene* createScene();
    
    //enumを整数に変換
    static int convertLayerZPositionIntoInt(LayerZPosition type);
};

#endif /* defined(__Solitaire__GameMain__) */
