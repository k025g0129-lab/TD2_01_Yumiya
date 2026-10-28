#pragma once
#include "KamataEngine.h"
#include"Rect.h"

// 前方宣言
class Player;

enum class CameraMode {
	kFollow,       // プレイヤー追従
	kForcedScroll, // 強制スクロール
	kClear,
};

class CameraController {
public:
	void Initialize(KamataEngine::Camera* camera_);
	void Update();
	void Reset();

	// カメラシェイク開始
	void StartShake(float power, int32_t durationFrames);

	// カメラシェイクだけ更新
	void UpdateShake();

	//============================================================
	// ゲッター・セッター
	//============================================================

	CameraMode GetMode() const { return mode_; }

	void SetTarget(Player* target) { target_ = target; }
	void SetMode(CameraMode mode) { mode_ = mode; }
	void SetMovableArea(Rect area) { movableArea_ = area; }
	void SetMovableArea(float right = kBaseRight, float left = kBaseLeft, float bottom = kBaseBottom, float top = kBaseTop) {
		movableArea_.left = left;
		movableArea_.right = right;
		movableArea_.bottom = bottom;
		movableArea_.top = top;
	}

	// クリア用カメラへの接近開始
	void StartClearApproach();

	// クリアカメラの接近が完了したか
	bool IsClearApproachFinished() const { return clearCameraTimer_ >= kClearCameraDuration; }

private:
	// カメラ
	KamataEngine::Camera *camera_ = nullptr;

	// 追従対象のプレイヤー
	Player* target_ = nullptr;

	// カメラモード
	CameraMode mode_ = CameraMode::kFollow;

	// 追従カメラ更新
	void UpdateFollow();

	// 追従対象とカメラの座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -25.0f};

	// カメラ移動範囲
	Rect movableArea_ = {};

	// カメラの目標座標
	KamataEngine::Vector3 targetCameraPosition_ = {};

	//速度掛け算
	static inline const float kVelocityBias_ = 30.0f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect kMargin_ = {-10.0f, 10.0f, -5.5f, 5.5f};


	//============================================================
	// 強制スクロール
	//============================================================

	// 強制スクロール更新
	void UpdateForcedScroll();

	// 強制スクロール速度
	static inline const float kForcedScrollSpeed = 0.05f;

	//============================================================
	// クリアカメラ
	//============================================================

	// クリアカメラ更新
	void UpdateClearApproach();

	// 接近開始時のカメラ座標
	KamataEngine::Vector3 clearCameraStartPosition_ = {};

	// 接近先のカメラ座標
	KamataEngine::Vector3 clearCameraEndPosition_ = {};

	// 接近経過時間
	float clearCameraTimer_ = 0.0f;

	// 接近時間
	static inline constexpr float kClearCameraDuration = 0.6f;

	// 1フレームの経過時間
	static inline constexpr float kCameraDeltaTime = 1.0f / 60.0f;

	// プレイヤーから見たクリアカメラの位置
	static inline const KamataEngine::Vector3 kClearCameraOffset = {0.0f, 1.5f, -10.0f};

	//============================================================
	// カメラシェイク
	//============================================================

	// シェイク処理内部
	void ApplyShake();

	// 前フレームで適用した揺れの移動量
	KamataEngine::Vector3 shakeOffset_ = {};

	// シェイクの強さ
	float shakePower_ = 0.0f;

	// シェイクの残りフレーム数
	int32_t shakeTimer_ = 0;

	// シェイクの持続フレーム数
	int32_t shakeDuration_ = 0;

	// シェイク用の経過フレーム数
	int32_t shakeElapsedFrames_ = 0;

	//============================================================
	// 移動範囲の基礎値
	//============================================================

	static inline const float kBaseLeft = 30.0f;
	static inline const float kBaseRight = 100.0f;
	static inline const float kBaseBottom = 10.7f;
	static inline const float kBaseTop = 10.7f;
};
