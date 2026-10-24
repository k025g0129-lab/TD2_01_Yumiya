#pragma once
#include"PlayerDirection.h"
#include "Collision.h"
#include <memory>
#include <vector>
#include <KamataEngine.h>

// 前方宣言
class BulletTrailParticles;

class PlayerBullet {
  public:
	/// ===============================================
	/// ゲッター
	/// ===============================================

	/// <summary>
	/// 位置の取得
	/// </summary>
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	/// <summary>
	/// カメラ内にいるかの取得
	/// </summary>
	bool GetisInCamera() const { return isInCamera_; }

	// あたり判定の取得
	OBB GetOBB() const;

	// 命中しているか
	bool IsHit() const { return isHit_; }

	int GetAttack() const { return kAttackTable[chargeStep_]; }

	/// ===============================================
	/// セッター
	/// ===============================================

	/// <summary>
	/// 方向の設定
	/// </summary>
	void SetDirection(Direction direction) { direction_ = direction; }

	// 命中時の処理
	void OnCollision() { isHit_ = true; }

	// 軌跡パーティクル管理先を設定
	void SetTrailParticles(BulletTrailParticles* trailParticles) {
		trailParticlesManager_ = trailParticles;
	}

	/// ===============================================
	/// public関数
	/// ===============================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerBullet();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerBullet();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Camera* camera, KamataEngine::Vector3 position, Direction direction, int chargeStep);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	/// ===============================================
	/// private変数
	/// ===============================================

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// チャージ段階に応じた速度定数
	static inline const float kSpeedTable[] = {0.1f, 0.2f, 0.4f};

	// 方向
	Direction direction_ = Direction::kRight;

	// カメラ内にいるか
	bool isInCamera_ = true;

	// 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 命中フラグ
	bool isHit_ = false;

	// この矢のチャージ段階
	int chargeStep_ = 0;

	// チャージ段階に対応した攻撃力
	static inline const int kAttackTable[] = {1, 3, 5};


	//============================================================
	// 矢の軌跡パーティクル
	//============================================================

	// パーティクル
	BulletTrailParticles* trailParticlesManager_ = nullptr;

	// パーティクル生成間隔（フレーム）
	static inline constexpr int kParticleEmitInterval = 4;

	// パーティクル生成回数
	int particleEmissionCount_ = 0;

	// パーティクル生成用カウンター
	int particleEmitFrame_ = 0;

	// パーティクル生成要求
	void EmitParticles();

	/// ===============================================
	/// private関数
	/// ===============================================

	/// <summary>
	/// 方向から速度を算出する
	/// </summary>
	void DirectionToVelocity();

	/// <summary>
	/// 方向から角度を算出する
	/// </summary>
	void DirectionToRotate();
};