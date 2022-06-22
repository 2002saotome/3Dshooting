#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <time.h>

using namespace DirectX;

//コンストラクタ
GameScene::GameScene() {}

//デストラクタ
GameScene::~GameScene() {
	delete spriteBG_;       // BG
	delete modelStage_;     //ステージ
	delete modelPlayer_;    //プレイヤー
	delete spriteTitle_;    //タイトル
	delete spriteGameOver_; //ゲームオーバー
}

//初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//乱数初期化
	srand((unsigned int)time(NULL));

	// BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	//ビュープロダクションの初期化
	viewProjection_.Initialize();

	//ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	for (int i=0;i<20;i++) 
	{
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};
		worldTransformStage_[i].Initialize();
	}
	

	//ビュープロダクションの初期化
	viewProjection_.eye = {0, 1, -6};
	viewProjection_.target = {0, 1, 0};
	viewProjection_.Initialize();

	modelStage_ = Model::Create();
	/*worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	worldTransformStage_.Initialize();*/

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	for (int e = 0; e < 10; e++) {
		worldTransformBeam_[e].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformBeam_[e].Initialize();
	}

	//敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}

	//タイトル(2Dスプライト)
	texttureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(texttureHandleTitle_, {0, 0});

	//タイトル(エンター)
	texttureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(texttureHandleEnter_, {400, 400});

	//ゲームオーバー(2Dスプライト)
	texttureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(texttureHandleGameOver_, {0, 0});

	//サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");

	//タイトルBGMを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
}

//ゲーム開始
void GameScene::GamePlayStart() 
{
	gameScore_ = 0;  //スコア
	playerLife_ = 3; //プレイヤーライフ
	gameScore_ = 0; //ゲームタイマー
	for (int e = 0; e < 10; e++) {
		beamFlag_[e] = 0; //ビームフラグ
	}
	for (int i = 0; i < 10; i++) {
		enemyFlag_[i] = 0; // 敵フラグ
	}

	// BGM切り替え
	audio_->StopWave(voiceHandleBGM_); // BGM停止
	//ゲームプレイを再生
	voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
}

//*******************************************************
//タイトル
//*******************************************************
//タイトル更新
void GameScene::TitleUpdate() {
	//エンターキーを押した瞬間
	if (input_->TriggerKey(DIK_RETURN))
	{
		//モードをゲームプレイへ変更
		sceneMode_ = 0;
		GamePlayStart();

		//BGM切り替え
		audio_->StopWave(voiceHandleBGM_);  //BGM停止
		//タイトルの音楽を再生
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGamePlayBGM_, true);
	}
}

//タイトル表示2D
void GameScene::TitleDraw2DNear() {}

void GameScene::TitleDraw2DBack() {
	//タイトル表示
	spriteTitle_->Draw();

	if (gameTimer_ % 40 >= 20) {
		//エンター表示
		spriteEnter_->Draw();
	}
}

void GameScene::GameOverUpdate() 
{

	if (input_->TriggerKey(DIK_RETURN)) 
	{
		sceneMode_ = 1;
	}
}

void GameScene::GameOverDraw2DNear() 
{
	//ゲームオーバー表示
	spriteGameOver_->Draw();

	if (gameTimer_ % 40 >= 20)
	{
		//エンターキーで表示
		spriteEnter_->Draw();
	}
}

//プレイヤー更新
void GameScene::playerUpdate() {
	//移動

	//右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}

	//左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

	//移動制限

	//右移動制限
	if (worldTransformPlayer_.translation_.x >= 4) {
		worldTransformPlayer_.translation_.x = 4;
	}

	//左移動制限
	if (worldTransformPlayer_.translation_.x <= -4) {
		worldTransformPlayer_.translation_.x = -4;
	}

	//行列更新
	worldTransformPlayer_.UpdateMatrix();
}

//更新
void GameScene::Update() {
	//各シーンの更新処理を呼び出す
	switch (sceneMode_) {
	case 0:
		//ゲームプレイ更新
		GamePlayUpdate();
		break;
	case 1:
		TitleUpdate(); //タイトル更新
		gameTimer_ += 1;
		break;
	case 2:
		GameOverUpdate();
		gameTimer_ += 1;
		break;
	}
}

//ビーム
//***************************************

//ビーム更新
void GameScene::BeamUpdate() {
	//移動
	BeamMove();

	//ビーム発生
	BeamBorn();

	for (int e = 0; e < 10; e++) {
		//行列更新
		worldTransformBeam_[e].UpdateMatrix();
	}
}

//ビーム移動
void GameScene::BeamMove() {
	for (int e = 0; e < 10; e++) {
		if (beamFlag_[e] == 1) {
			//奥移動
			worldTransformBeam_[e].translation_.z += 0.5f;
		}
		if (worldTransformBeam_[e].translation_.z > 40) {
			//存在しない
			beamFlag_[e] = 0;
		}
		//回転
		worldTransformBeam_[e].rotation_.x += 0.1f;
	}
}

//ビーム発生(発射)
void GameScene::BeamBorn() {
	if (beamTimer_ == 0) {
		for (int e = 0; e < 10; e++) {

			if (beamFlag_[e] == 0) {
				if (input_->PushKey(DIK_SPACE)) {
					worldTransformBeam_[e].translation_.x = worldTransformPlayer_.translation_.x;
					worldTransformBeam_[e].translation_.z = worldTransformPlayer_.translation_.z;

					//存在する
					beamFlag_[e] = 1;
					//タイマーフラグ1にする
					beamTimer_ = 1;

					//ループ終了
					break;
				}
			}
		}
	} else {
		beamTimer_++;
		if (beamTimer_ > 10) {
			beamTimer_ = 0;
		}
	}
}

//敵
//**********************************

//敵更新
void GameScene::EnemyUpdate() {
	//移動
	EnemyMove();

	EnemyBorn();

	EnemyJump();

	//行列更新
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].UpdateMatrix();
	}
}

//敵移動
void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) 
	{
		if (enemyFlag_[i] == 1) 
		{
			//手前移動
			worldTransformEnemy_[i].translation_.z -= 0.5f;

			//タイマーにより速度を設定
			worldTransformEnemy_[i].translation_.z -= 0.5f;
			worldTransformEnemy_[i].translation_.z -= gameTimer_ / 10000.0f;

			//手前まできたら奥に戻す
			if (worldTransformEnemy_[i].translation_.z < -10.0f) {
				enemyFlag_[i] = 0;
			}
			//回転
			worldTransformEnemy_[i].rotation_.x -= 0.1f;

			worldTransformEnemy_[i].translation_.x += enemySpeed_[i];

			//画面の端までいったらスピードを逆
			if (worldTransformEnemy_[i].translation_.x > 5) {
				enemySpeed_[i] = -0.2f;
			}

			if (worldTransformEnemy_[i].translation_.x < -5) {
				enemySpeed_[i] = 0.2f;
			}
		}
	}
}

//敵発生
void GameScene::EnemyBorn() {
	//乱数で発生
	if (rand() % 10 == 0) {
		for (int i = 0; i < 10; i++) {
			if (enemyFlag_[i] == 0) {
				enemyFlag_[i] = 1;
				worldTransformEnemy_[i].translation_.y = 0;
				worldTransformEnemy_[i].translation_.z = 40.0f;

				//乱数でX座標の指定
				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;
				worldTransformEnemy_[i].translation_.x = x2;

				//敵スピード
				if (rand() % 2 == 0) {
					enemySpeed_[i] = 0.2f;
				} else {
					enemySpeed_[i] = -0.2f;
				}

				//ループ終了
				break;
			}
		}
	}
}

//*********************************************************
//衝突判定
//*********************************************************

//衝突判定

void GameScene::Collision() {
	//衝突判定(プレイヤーと敵)
	CollisionplayerEnemy();
	CollisionBeamEnemy();
}

//衝突判定(プレイヤーと敵)
void GameScene::CollisionplayerEnemy() {
	for (int i = 0; i < 10; i++) {
		//敵が存在するならば
		if (enemyFlag_[i] == 1) {
			//差を求める
			float dx =
			  abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
			float dz =
			  abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

			//衝突したら
			if (dx < 1 && dz < 1) {
				//存在しない
				enemyFlag_[i] = 0;

				//プレイヤーライフ-1
				playerLife_--;

				//プレイヤーヒットSE
				audio_->PlayWave(soundDataHandlePlayerHitSE_);
			}
		}
	}
}

//衝突判定(ビームと敵)
void GameScene::CollisionBeamEnemy() {
	for (int e = 0; e < 10; e++) {
		//敵が存在するならば
		if (enemyFlag_[e] == 1) 
		{
			for (int b = 0; b < 10; b++) 
			{
				if (beamFlag_[b] ==1) 
				{
				//差を求める
				float dX = abs(
				  worldTransformBeam_[b].translation_.x - worldTransformEnemy_[e].translation_.x);
				float dZ = abs(
				  worldTransformBeam_[b].translation_.z - worldTransformEnemy_[e].translation_.z);

				//衝突したら
				if (dX < 1 && dZ < 1) {
					//フラグを2にする
					enemyFlag_[e] = 2;

					//敵のジャンプスピードを1にする
					enemyjumpSpeed_[e] = 1;

					//スコア＋1
					gameScore_++;

					//敵ヒットSE
					audio_->PlayWave(soundDataHandleEnemyHitSE_);
				}
				}
			}
		}
	}
}

//敵ジャンプ
void GameScene::EnemyJump() 
{
 //敵ループ
	for (int e=0;e<10;e++) 
	{
     //消滅演出ならば
		if (enemyFlag_[e]==2) 
		{
	      //移動
			worldTransformEnemy_[e].translation_.y += enemyjumpSpeed_[e];

			//速度を減らす
			enemyjumpSpeed_[e] -= 0.1f;

			//斜め移動
			worldTransformEnemy_[e].translation_.x += enemySpeed_[e] * 2;

			//下に落ちると消滅
			if (worldTransformEnemy_[e].translation_.y<-3)
			{
				enemyFlag_[e] = 0; //存在しない
			}

		}
	}
}

void GameScene::StageUpdate() 
{
//各ステージでループ
	for (int i=0;i<20;i++) 
	{
		//手前に移動
		worldTransformStage_[i].translation_.z -= 0.1f;
       //端まで来たら奥へ戻る
		if (worldTransformStage_[i].translation_.z<-5) 
		{
			worldTransformStage_[i].translation_.z += 40;
		}
		//行列更新
		worldTransformStage_[i].UpdateMatrix();
	}
}


//ゲームプレイ座標
void GameScene::GamePlayUpdate() 
{
	//プレイヤー更新
	playerUpdate();
	//ステージ更新
	StageUpdate();
	//敵更新
	EnemyUpdate();
	//ビーム更新
	BeamUpdate();
	//衝突判定
	Collision();

	if (playerLife_ == 0)
	{
		// BGM切り替え
		audio_->StopWave(voiceHandleBGM_); // BGM停止
		//ゲームオーバーを再生
		voiceHandleBGM_ = audio_->PlayWave(soundDataHandleGameOverBGM_, true);
		sceneMode_ = 2;
	}
}
//****************************************************
//表示
//****************************************************

//ゲームプレイ表示3D
void GameScene::GamePlayDraw3D() {
	//ステージ
	for (int i=0;i<20;i++) 
	{
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	
	//プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	//ビーム
	for (int e = 0; e < 10; e++) {
		if (beamFlag_[e] == 1) {
			modelBeam_->Draw(worldTransformBeam_[e], viewProjection_, textureHandleBeam_);
		}
	}
	//敵
	for (int e = 0; e < 10; e++)
	{
		if (enemyFlag_[e] == 1 || enemyFlag_[e]==2)
		{
			modelEnemy_->Draw(worldTransformEnemy_[e], viewProjection_, textureHandleEnemy_);
		}
	}
}

//ゲームプレイ表示2D背景
void GameScene::GamePlayDraw2DBack() 
{
	//背景
	spriteBG_->Draw();
}

//ゲームプレイ表示2D近景
void GameScene::GamePlayDraw2DNear() 
{
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);

	sprintf_s(str, "LIFE %d", playerLife_);
	debugText_->Print(str, 900, 10, 2);
}

//表示
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	switch (sceneMode_) {
	case 0:
		//各シーンの背景2D表示を呼び出す
		GamePlayDraw2DBack();
		break;
	case 1:
		TitleDraw2DBack();
		break;
	case 2:
		GamePlayDraw2DBack();
		break;
	}

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる

	switch (sceneMode_) {
	case 0:
		GamePlayDraw3D(); //ゲームプレイ3D表示
		break;
	case 2:
		GamePlayDraw3D();
		break;
	}

	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) {
	case 0:
		//ゲームプレイ2D近景表示
		GamePlayDraw2DNear();
		break;
	case 1:
		TitleDraw2DNear();
		break;
	case 2:
		GamePlayDraw2DNear();
		GameOverDraw2DNear();
		break;
	}
	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
