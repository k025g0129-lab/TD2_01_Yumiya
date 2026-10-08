#pragma once

#include <KamataEngine.h>

class PlayerBullet {
public:
	/// ===============================================
	/// 列挙型・構造体
	/// ===============================================

	// 方向
	enum class Direction { kUp = 0, kRightUp = 1, kRight = 2, kRightDown = 3, kDown = 4 };

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
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, KamataEngine::Vector3 position, Direction direction);

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