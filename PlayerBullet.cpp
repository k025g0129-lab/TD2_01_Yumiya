#include "PlayerBullet.h"

#include "Matrix4x4Util.h"
#include "ModelManager.h"

#include <assert.h>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

OBB PlayerBullet::GetOBB() const {
	OBB result{};

	result.center = GetPosition();
	result.size = {kWidth / 2.0f, kHeight / 2.0f, 0.01f};

	result.UpdateOBBOrientations(worldTransform_.rotation_);

	return result;
}

PlayerBullet::PlayerBullet() {}

PlayerBullet::~PlayerBullet() {}

void PlayerBullet::Initialize(Camera* camera, Vector3 position, Direction direction, int chargeStep) {
	// ポインタの引き渡しとエラー処理
	if (camera == nullptr) {
		assert(false && "PlayerBulletのCameraのポインタがNULLです");
	} else {
		camera_ = camera;
	}

	model_ = ModelManager::GetInstance()->Load("Arrow", true);

	assert(model_);

	// ワールド変換データの初期化
	worldTransform_.Initialize();

	// 初期座標の設定
	worldTransform_.translation_ = position;

	// 方向の初期化
	direction_ = direction;

	// 角度の設定
	DirectionToRotate();

	// 加速度の設定
	DirectionToAcceleration();

	// 攻撃力の設定
	attack_ = kStepZeroAttack;

	if (chargeStep == 1) {
		attack_ = kStepOneAttack;
	} else if (chargeStep >= 2) {
		attack_ = kStepTwoAttack;
	}
}

void PlayerBullet::Update() {
	// 位置に速度を加算
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// カメラ内にいるか判定(仮置き処理)
	if (std::abs(worldTransform_.translation_.x - camera_->translation_.x) >= 15.0f || std::abs(worldTransform_.translation_.y - camera_->translation_.y) >= 10.0f) {
		isInCamera_ = false;
	}

	// ワールド変換データの更新
	WorldTransformUpdate(worldTransform_);
}

void PlayerBullet::Draw() {
	// この辺にチャージ段階に応じて異なるパーティクル出す処理とか書く

	// カメラ内にいる場合描画
	if (isInCamera_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void PlayerBullet::DirectionToAcceleration() {
	// 斜め方向による減衰率
	float kDiagonalSpeedDecay = 0.7f;

	// 速度の設定
	switch (direction_) {
	case Direction::kUp:
		velocity_ = {0.0f, kSpeed, 0.0f};
		break;

	case Direction::kRightUp:
		velocity_ = {kSpeed * kDiagonalSpeedDecay, kSpeed * kDiagonalSpeedDecay, 0.0f};
		break;

	case Direction::kRight:
		velocity_ = {kSpeed, 0.0f, 0.0f};
		break;

	case Direction::kRightDown:
		velocity_ = {kSpeed * kDiagonalSpeedDecay, -kSpeed * kDiagonalSpeedDecay, 0.0f};
		break;

	case Direction::kDown:
		velocity_ = {0.0f, -kSpeed, 0.0f};
		break;

	default:
		velocity_ = {kSpeed, 0.0f, 0.0f};
	}
}

void PlayerBullet::DirectionToRotate() {
	// y軸は固定
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	// z軸の設定
	switch (direction_) {
	case Direction::kUp:
		worldTransform_.rotation_.z = std::numbers::pi_v<float> / 2.0f;
		break;

	case Direction::kRightUp:
		worldTransform_.rotation_.z = std::numbers::pi_v<float> / 4.0f;
		break;

	case Direction::kRightDown:
		worldTransform_.rotation_.z = -std::numbers::pi_v<float> / 4.0f;
		break;

	case Direction::kDown:
		worldTransform_.rotation_.z = -std::numbers::pi_v<float> / 2.0f;
		break;

	default:
		worldTransform_.rotation_.z = 0.0f;
	}
}