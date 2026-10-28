#pragma once
#include"KamataEngine.h"
#include"Player.h"
#include "BaseEnemy.h"
#include "Skydome.h"
#include<vector>
#include <memory>
#include "MapChipField.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Fade.h"
#include <list>
#include"BaseEffect.h"
#include "GameHUD.h"
#include "IScene.h"
#include "BulletTrailParticles.h"

// 前方宣言
class HitEffect;
class StageManager;

// ゲームシーン
class GameScene : public IScene {
  public:
	// 初期化処理
	void Initialize() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Draw() override;

	void GenerateFieldObjects();

	GameScene();
	~GameScene();



	// ヒットエフェクトを生成
	void CreateHitEffect(const KamataEngine::Vector3& position);

	// ガードエフェクトを生成
	void CreateGuardEffect(const KamataEngine::Vector3& position);


	// 現在のウェーブを取得
	int GetCurrentWave() const { return currentWave_; }

	// ウェーブ残り時間を取得
	float GetWaveRemainingTime() const { return kWaveDuration - waveTimer_; }

	// ウェーブの制限時間（秒）
	static inline constexpr float kWaveDuration = 30.0f;

  private:

	// プレイヤー
	std::unique_ptr<Player> player_;

	// 矢の軌跡パーティクル
	std::unique_ptr<BulletTrailParticles> bulletTrailParticles_;

	// 敵
	std::list<std::unique_ptr<BaseEnemy>> enemies_;

	// 天球
	std::unique_ptr<Skydome> skydome_;

	// デスパーティクル
	std::unique_ptr<DeathParticles> deathParticles_;

	// マップチップフィールド
	std::unique_ptr<MapChipField> mapChipField_;

	// カメラ
	std::unique_ptr<KamataEngine::Camera> camera_;
	std::unique_ptr<CameraController> cameraController_;

	// デバッグカメラ
	std::unique_ptr<KamataEngine::DebugCamera> debugCamera_;

	// デバッグカメラの有効化フラグ
	bool isDebugCameraActive_ = false;

	// HUD
	std::unique_ptr<GameHUD> gameHUD;

	// ゲームのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kPlay,    // ゲームプレイ
		kDeath,   // デス演出
		kFadeOut, // フェードアウト
	};

	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	// フェード
	std::unique_ptr<Fade> fade_;

	// 全てのあたり判定を行う
	void CheckAllCollisions();

	// ゲームプレイフェーズの更新
	void UpdatePlayPhase();

	// デス演出フェーズの更新
	void UpdateDeathPhase();

	// フェーズ切り替え
	void ChangePhase();

	// ヒットエフェクト
	std::list<std::unique_ptr<BaseEffect>> effects_;

	// ヒットエフェクトの更新
	void UpdateHitEffects();

	// ヒットエフェクトの描画
	void DrawHitEffects();

	// 敵を生成
	void GenerateEnemy(KamataEngine::Vector3 &position,uint8_t type);

	std::vector<std::vector<std::unique_ptr<KamataEngine::WorldTransform>>> worldTransformBlocks_;

	KamataEngine::Model* modelBlock_ = nullptr;

	//============================================================
	// ウェーブ管理
	//============================================================

	// 現在のウェーブ（1から開始）
	int currentWave_ = 1;

	// 現在のウェーブの経過時間（秒）
	float waveTimer_ = 0.0f;

	// 1フレームの時間
	static inline constexpr float kWaveDeltaTime = 1.0f / 60.0f;

	// ウェーブを更新する
	void UpdateWave();

	// 次のウェーブに移行する
	void AdvanceWave();

	// ウェーブ切り替え時のリセット
	void ClearWave();

	// ウェーブに応じた敵を生成する
	void SpawnWaveEnemies();

	// そのウェーブの全敵数
	int totalEnemyCount_ = 0;



	// 画面サイズ
	const int kMaxWindowWidth = 1280;
	const int kMaxWindowHeight = 720;
};
