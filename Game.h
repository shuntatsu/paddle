#pragma once
#include <SDL2/SDL.h>
#include <vector>

// 座標値
struct Vector2{
    float x;
    float y;
};

// ボールの構造体
struct Ball{
    Vector2 mBallPos;
    Vector2 mBallVel;
};

class Game{
public:
    // コンストラクタ
    Game();

    // ゲームを初期化
    bool Initialize();

    // ゲームオーバーまでゲームループを実行
    void RunLoop();

    // ゲームを終了
    void Shutdown();

private:
    // ゲームループのためのヘルパー関数
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput(); // 出力生成

    // SDL が作るウィンドウ
    SDL_Window* mWindow;

    // SDLのグラフィック用コード
    SDL_Renderer* mRenderer;

    // (符号なし32ビット整数を表すデータ型)時間の計測
    Uint32 mTicksCount;
 
    // ゲームの続行を指示
    bool mIsRunning;
 
    // パドル方向(direction)
    int mPaddleDir1;
    int mPaddleDir2;

    // パドル位置
    Vector2 mPaddlePos1;
    Vector2 mPaddlePos2;

    // ボールの位置
    Vector2 mBallPos;

    // ボールの速度(velocity)
    Vector2 mBallVel;

    // ボール
    std::vector<Ball> mBall;
};