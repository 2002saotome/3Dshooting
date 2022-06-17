#pragma once

#include "Audio.h"
#include "DebugText.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXMath.h>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	//ビュープロダクション(共通)
	ViewProjection viewProjection_;

	//ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_;

	//プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	//ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_[10];

	//敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];

	//タイトル(スプライト)
	uint32_t texttureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	uint32_t texttureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	uint32_t texttureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;
	//プレイヤー更新
	void playerUpdate();

	void BeamUpdate(); //ビーム更新
	void BeamMove();   //ビーム移動
	void BeamBorn();   //ビーム発生

	void EnemyUpdate(); //敵更新
	void EnemyMove();   //敵移動

	void EnemyBorn(); //敵発生

	void Collision();            //衝突判定
	void CollisionplayerEnemy(); //衝突判定(プレイヤーと敵)
	void CollisionBeamEnemy();   //衝突判定(ビームと敵)

	void GamePlayUpdate();
	void GamePlayDraw3D();
	void GamePlayDraw2DBack();
	void GamePlayDraw2DNear();

	void TitleUpdate();     //タイトル更新
	void TitleDraw2DNear(); //タイトル2D
	void TitleDraw2DBack();

	void GameOverDraw2DNear();
	void GameOverUpdate();

	void GamePlayStart(); //ゲーム開始

	int gameScore_ = 0; //ゲームスコア

	int playerLife_ = 3;

	int enemyFlag_[10] = {}; //敵存在フラグ

	int sceneMode_ = 1; //シーンモード(0:ゲームプレイ 1:タイトル)

	int gameTimer_ = 0;

	int beamFlag_[10] = {}; //ビーム存在フラグ

	int beamTimer_ = 0; //ビーム発射タイマー

	float enemySpeed_[10] = {}; //敵スピード
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
