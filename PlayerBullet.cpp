#include "PlayerBullet.h"

#include "Matrix4x4Util.h"
#include "ModelManager.h"
#include "BulletTrailParticles.h"
#include <assert.h>
#include <cmath>
#include <numbers>
#include <algorithm>

using namespace KamataEngine;

OBB PlayerBullet::GetOBB() const {
	OBB result{};

	result.center = GetPosition();
	result.size = {kWidth / 2.0f, kHeight / 2.0f, 0.01f};

	result.UpdateOBBOrientations(worldTransform_.rotation_);

	return result;
}

bool PlayerBullet::HasHitEnemy(const BaseEnemy* enemy) const {
	// 命中済みの敵を検索
	return std::find(hitEnemies_.begin(), hitEnemies_.end(), enemy) != hitEnemies_.end();
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

	// チャージ段階を保存
	chargeStep_ = std::clamp(chargeStep, 0, 2);

	// 加速度の設定
	DirectionToVelocity();

	// パーティクル発生管理
	particleEmitFrame_ = 0;
	particleEmissionCount_ = 0;

	// 残り命中可能回数の初期化
	remainingHits_ = kHitCountTable[chargeStep_];

	// 命中履歴の初期化
	hitEnemies_.clear();
}

void PlayerBullet::Update() {
	// 位置に速度を加算
	worldTransform_.translation_ += velocity_;

	// カメラ内にいるか判定(仮置き処理)
	if (std::abs(worldTransform_.translation_.x - camera_->translation_.x) >= 20.0f || std::abs(worldTransform_.translation_.y - camera_->translation_.y) >= 10.0f) {
		isInCamera_ = false;
	}

	// 軌跡の新規生成を要求
	EmitParticles();

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

void PlayerBullet::OnCollision(BaseEnemy* enemy) {

	// 既に命中可能回数を使い切っていたら終了
	if (IsFinished()) {
		return;
	}

	// 同じ敵に命中済みなら終了
	if (HasHitEnemy(enemy)) {
		return;
	}

	// 命中した敵を記録
	hitEnemies_.push_back(enemy);

	// 残り命中可能回数を減らす
	remainingHits_--;
}

void PlayerBullet::EmitParticles() {

	// パーティクル管理先がなければ生成しない
	if (trailParticlesManager_ == nullptr) {
		return;
	}

	// カメラ外では生成しない
	if (!isInCamera_) {
		return;
	}

	// 発生間隔
	particleEmitFrame_++;

	if (particleEmitFrame_ < kParticleEmitInterval) {
		return;
	}

	particleEmitFrame_ = 0;

	// GameSceneが所有する管理クラスへ生成を依頼
	trailParticlesManager_->Emit(worldTransform_.translation_, velocity_, chargeStep_, particleEmissionCount_);

	// 次の生成時の配置を変化させる
	particleEmissionCount_++;
}

void PlayerBullet::DirectionToVelocity() {
	// 斜め方向による減衰率
	float kDiagonalSpeedDecay = 0.7f;

	float bulletSpeed = kSpeedTable[chargeStep_];

	// 速度の設定
	switch (direction_) {
	case Direction::kUp:
		velocity_ = {0.0f, bulletSpeed, 0.0f};
		break;

	case Direction::kRightUp:
		velocity_ = {bulletSpeed * kDiagonalSpeedDecay, bulletSpeed * kDiagonalSpeedDecay, 0.0f};
		break;

	case Direction::kRight:
		velocity_ = {bulletSpeed, 0.0f, 0.0f};
		break;

	case Direction::kRightDown:
		velocity_ = {bulletSpeed * kDiagonalSpeedDecay, -bulletSpeed * kDiagonalSpeedDecay, 0.0f};
		break;

	case Direction::kDown:
		velocity_ = {0.0f, -bulletSpeed, 0.0f};
		break;

	default:
		velocity_ = {bulletSpeed, 0.0f, 0.0f};
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