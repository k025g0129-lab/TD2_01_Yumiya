#include "EnemyBehavior.h"
#include "Enemy.h"
#include "Matrix4x4Util.h"
#include <algorithm>
#include "Ease.h"

using namespace KamataEngine;

EnemyWalk::EnemyWalk(Enemy* enemy) { enemy_ = enemy; }

void EnemyWalk::Initialize() {
	velocty_ = {-kWalkSpeed, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
}

void EnemyWalk::Update() {
	enemy_->Move(velocty_);
}

void EnemyWalk::UpdateMotion() {
	walkTimer_ += 1.0f / 60.0f;

	// タイマーが大きくなりすぎないようにする
	if (walkTimer_ >= kWalkMotionTime) {
		walkTimer_ -= kWalkMotionTime;
	}

	// -1.0f ～ 1.0f を周期的に繰り返す値
	float param = std::sin(2.0f * kPi * walkTimer_ / kWalkMotionTime);

	// -1.0f ～ 1.0f を 0.0f ～ 1.0f に変換
	float t = (param + 1.0f) / 2.0f;

	// 最初の角度から最後の角度まで補間
	float angle = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;

	// X軸周りに回転アニメーション
	enemy_->SetRotationX(angle);
}

EnemyDeath::EnemyDeath(Enemy* enemy) { enemy_ = enemy; }

void EnemyDeath::Initialize() {
	deathTimer_ = 0.0f;

	// 死亡演出開始時の角度を保存
	deathStartRotationX_ = enemy_->GetRotationX();
}

void EnemyDeath::Update() {}

void EnemyDeath::UpdateMotion() {
	deathTimer_ += 1.0f / 60.0f;

	// デスタイマーを 0.0f ～ 1.0f に変換
	float t = std::clamp(deathTimer_ / kDeathMotionTime, 0.0f, 1.0f);

	// X軸回転で倒れる
	enemy_->SetRotationX(Lerp(deathStartRotationX_, kDeathMotionAngleEnd, Ease::EaseInOut(t)));

	// Y軸回転でぐるぐる回る
	enemy_->SetRotationY(enemy_->GetRotationY() + kDeathSpinSpeed * (1.0f / 60.0f));

	if (deathTimer_ >= kDeathMotionTime) {
		enemy_->SetDead(true);
	}
}