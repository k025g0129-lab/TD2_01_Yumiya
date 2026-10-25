#pragma once
#include "KamataEngine.h"
#include"Rect.h"

// 前方宣言
class Player;

enum class CameraMode {
	kFollow,       // プレイヤー追従
	kForcedScroll, // 強制スクロール
};

class CameraController {
public:
	void Initialize(KamataEngine::Camera* camera_);
	void Update();
	void Reset();



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
	// 移動範囲の基礎値
	//============================================================

	static inline const float kBaseLeft = 30.0f;
	static inline const float kBaseRight = 100.0f;
	static inline const float kBaseBottom = 10.7f;
	static inline const float kBaseTop = 10.7f;
};
