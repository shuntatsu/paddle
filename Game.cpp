#include "Game.h"
#include <iostream>
#include <random>
#include <cstddef>
#include <chrono>   // 時間の計測用

// ボールを追加する間隔（30秒）
const std::chrono::seconds ballAddInterval(30);

// 前回ボールを追加した時間を記録する変数
auto lastBallAddTime = std::chrono::steady_clock::now();

// ボールを追加する回数(最大)
const int numBallsToAdd = 5;
// 現在のボール数
int nowBallCount = 1;

// 壁の厚み(調整するかも)
const int thickness = 15;
const float paddleH = 100.0f;

// コンストラクト
Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleDir1(0)
,mPaddleDir2(0)
{
   
}

bool Game::Initialize(){

    // SDL 初期化
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);

    if(sdlResult != 0){
        SDL_Log("SDL を初期化できません : %s", SDL_GetError());
        return false;
    }

    // SDL Window 作成
    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (第一章)",
        100,   // x 座標
        100,   // y 座標
        1024,  // ウィンドウの幅  (Width)
        768,   // ウィンドウの高さ(height)
        0      // フラグ
    );

    if(!mWindow){
        SDL_Log("ウィンドウを作成できません : %s", SDL_GetError());
    }

    // SDL renderer (レンダラー)制作
    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,       // グラフィックドライバーを指定する、-1にすればSDLが決めてくれる
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if(!mRenderer){
        SDL_Log("レンダラーを生成できません : %s", SDL_GetError());
    }

    // ランダム数生成
    std::random_device rd;  // 真の乱数生成器
    std::mt19937 mt(rd());  // メルセンヌ・ツイスタ法に基づく擬似乱数生成器
    std::uniform_int_distribution<int> dist_float(-20.0f, 20.0f); // 範囲 [1, 100] の一様分布

    // ランダムな数を生成して表示する
    float randomNum = dist_float(mt);

    // オブジェクト初期化
    mPaddlePos1.x = 10.0f;
    mPaddlePos1.y = 768.0f/2.0f;
    mPaddlePos2.x = 1024.0f - 10.0f;
    mPaddlePos2.y = 768.0f/2.0f;

    for (int i = 0; i < numBallsToAdd; ++i) {
        // ボールの追加
        Ball newBall;
        // 新しいボールの位置や速度を設定
        newBall.mBallPos.x = 1024.0f/2.0f;
        newBall.mBallPos.y = 768.0f/2.0f;
        newBall.mBallVel.x = -200.0f + randomNum;
        newBall.mBallVel.y = 235.0f + randomNum;

        // 新しいボールをmBallに追加する
        mBall.push_back(newBall);
    }
    return true;
}

void Game::RunLoop(){
    // 実行ループ
    while(mIsRunning){
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput(){
    // イベントポイント
    SDL_Event event;

    // キューにイベントがあれば繰り返す
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                mIsRunning = false;
                break;
        }
    }

    // キーボード初期設定
    const Uint8* state = SDL_GetKeyboardState(NULL);
    // 「ESC」設定
    if(state[SDL_SCANCODE_ESCAPE]){
        mIsRunning = false;
    }

    // パドル移動1
    mPaddleDir1 = 0;
    if(state[SDL_SCANCODE_W]){
        mPaddleDir1 -= 1;
    }
    if(state[SDL_SCANCODE_S]){
        mPaddleDir1 += 1;
    }

    // パドル移動2
    mPaddleDir2 = 0;
    if(state[SDL_SCANCODE_I]){
        mPaddleDir2 -= 1;
    }
    if(state[SDL_SCANCODE_K]){
        mPaddleDir2 += 1;
    }
}

void Game::UpdateGame(){
    // 前のフレームから16msが経過するのを待つ(60fps)
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
        ;

    // deltaTimeは前のフレームとの時刻差を秒に変換した値
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
    
    // デルタタイムの最大値を制限(遅くとも20fps処理)
    if(deltaTime > 0.05f){
        deltaTime = 0.05f;
    }

    // 時刻を更新
    mTicksCount = SDL_GetTicks();

    // TODO: ゲームワールドのオブジェクトを
    //       デルタタイム関数として更新
    if(mPaddleDir1 != 0){
        mPaddlePos1.y += mPaddleDir1 * 300.0f * deltaTime;
        // パドル1が画面から出ないようにする
        if(mPaddlePos1.y < (paddleH/2.0f + thickness)){
            mPaddlePos1.y = paddleH/2.0f + thickness;
        }
        if(mPaddlePos1.y > (768.0f - (paddleH/2.0f + thickness))){
            mPaddlePos1.y = 768.0f - (paddleH/2.0f + thickness);
        }
    }

    if(mPaddleDir2 != 0){
        mPaddlePos2.y += mPaddleDir2 * 300.0f * deltaTime;
        // パドル2が画面から出ないようにする
        if(mPaddlePos2.y < (paddleH/2.0f + thickness)){
            mPaddlePos2.y = paddleH/2.0f + thickness;
        }
        if(mPaddlePos2.y > (768.0f - (paddleH/2.0f + thickness))){
            mPaddlePos2.y = 768.0f - (paddleH/2.0f + thickness);
        }
    }

    // 現在の時間を取得
    auto currentTime = std::chrono::steady_clock::now();

    // 前回ボールを追加してからの経過時間を計算
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastBallAddTime);

    // 30秒経過したら新しいボールを追加
    if (elapsedTime >= ballAddInterval && nowBallCount < numBallsToAdd) {
        lastBallAddTime = currentTime;
        nowBallCount += 1;
    }

    // ボールの位置や速度にアクセスする
    for (int i = 0; i < nowBallCount; ++i) {

        // ボールの位置
        mBall[i].mBallPos.x += mBall[i].mBallVel.x * deltaTime;
        mBall[i].mBallPos.y += mBall[i].mBallVel.y * deltaTime;
        // 上の壁の衝撃処理
        if(mBall[i].mBallPos.y <= thickness && mBall[i].mBallVel.y < 0.0f){
            mBall[i].mBallVel.y *= -1;
        }
        // 下の壁の衝撃処理s
        if(mBall[i].mBallPos.y >= (768.0f - thickness) && mBall[i].mBallVel.y > 0.0f){
            mBall[i].mBallVel.y *= -1;
        }

        // パドルの衝撃処理
        // y の差分(絶対値を取得)
        float diff1 = mPaddlePos1.y - mBall[i].mBallPos.y;
        float diff2 = mPaddlePos2.y - mBall[i].mBallPos.y;
        diff1 = (diff1 > 0.0f) ? diff1 : -diff1;
        diff2 = (diff2 > 0.0f) ? diff2 : -diff2;
        if(
            // もし y の差が十分小さいなら
            diff1 <= paddleH/2.0f &&
            // x 軸の範囲内(-5.0f内)
            mBall[i].mBallPos.x <= 25.0f && mBall[i].mBallPos.x >= 20.0f &&
            // 左加速度
            mBall[i].mBallVel.x < 0.0f
        ){
            mBall[i].mBallVel.x *= -1;
        }else if(
            // もし y の差が十分小さいなら
            diff2 <= paddleH/2.0f &&
            // x 軸の範囲内(-5.0f内)
            mBall[i].mBallPos.x >= (1024.0f - 25.0f) && mBall[i].mBallPos.x <= (1024.0f - 20.0f) &&
            // 左加速度
            mBall[i].mBallVel.x > 0.0f
        ){
            mBall[i].mBallVel.x *= -1;
        }
        // 画面外へ出たとき、終了
        else if (mBall[i].mBallPos.x <= 0.0f || mBall[i].mBallPos.x >= 1024.0f)
    	{
            std::cout << "Ball " << i << " went out of bounds! mBallPos.x: " << mBall[i].mBallPos.x << " Exiting the game..." << std::endl;		   
            mIsRunning = false;
	    }
    }
}

void Game::GenerateOutput(){
    SDL_SetRenderDrawColor(
        mRenderer,
        0,   // resd
        0,   // green
        0, // blue
        255  // alpha(透明度) 255は完全透明
    );

    // バックバッファを現在の描画色でクリア
    SDL_RenderClear(mRenderer);

    // 背景色を白にする
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    // 長方形(壁)を描画するための構造体
    SDL_Rect wall{
        0,         // 左上隅の x
        0,         // 左上隅の y
        1024,      // 幅       w
        thickness  // 高さ     h
    };
    // 長方形を塗りつぶす(filled rectangle)
    // rect: 塗りつぶす矩形を表すSDL_Rect構造体へのポインタ。
    SDL_RenderFillRect(mRenderer, &wall);

    // 下壁描写
    wall.y = 768 - thickness;
    SDL_RenderFillRect(mRenderer, &wall);

    // パドル1描写
    SDL_Rect paddle1{
        static_cast<int>(mPaddlePos1.x),
        static_cast<int>(mPaddlePos1.y - paddleH/2),
        thickness,
        static_cast<int>(paddleH)
    };
    // パドル2描写
    SDL_Rect paddle2{
        static_cast<int>(mPaddlePos2.x),
        static_cast<int>(mPaddlePos2.y - paddleH/2),
        thickness,
        static_cast<int>(paddleH)
    };

    SDL_RenderFillRect(mRenderer, &paddle1);
    SDL_RenderFillRect(mRenderer, &paddle2);

    // ボールの位置や速度にアクセスする
    for (int i = 0; i < nowBallCount; ++i) {
        // ボールを描写するための構造体
        SDL_Rect ball{
            static_cast<int>(mBall[i].mBallPos.x + thickness/2),
            static_cast<int>(mBall[i].mBallPos.y - thickness/2),
            thickness,
            thickness
        };
        SDL_RenderFillRect(mRenderer, &ball);
    };

    // フロントバッファとバックバッファを交換
    SDL_RenderPresent(mRenderer);
}

void Game::Shutdown(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}
