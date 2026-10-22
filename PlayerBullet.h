#pragma once
#include"PlayerDirection.h"
#include "Collision.h"

#include <KamataEngine.h>

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

	// 命中時の処理
	void OnCollision() { isHit_ = true; }

	/// ===============================================
	/// セッター
	/// ===============================================

	/// <summary>
	/// 方向の設定
	/// </summary>
	void SetDirection(Direction direction) { direction_ = direction; }

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
	void Initialize(KamataEngine::Camera* camera, KamataEngine::Vector3 position, Direction direction);

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

	// 速度定数
	static inline const float kSpeed = 0.1f;

	// 方向
	Direction direction_ = Direction::kRight;

	// カメラ内にいるか
	bool isInCamera_ = true;

	// 当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// 命中フラグ
	bool isHit_ = false;

	/// ===============================================
	/// private関数
	/// ===============================================

	/// <summary>
	/// 方向から加速度を算出する
	/// </summary>
	void DirectionToAcceleration();

	/// <summary>
	/// 方向から角度を算出する
	/// </summary>
	void DirectionToRotate();
};