#include "EnemyBehavior.h"
#include "Enemy.h"
#include "Matrix4x4Util.h"
#include <algorithm>
#include "Ease.h"


using namespace KamataEngine;

EnemyWalk::EnemyWalk(BaseEnemy* enemy) { enemy_ = enemy; }

void EnemyWalk::Initialize() {
	velocty_ = {-kWalkSpeed, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
}

void EnemyWalk::Update() {
	enemy_->Move(velocty_);
}

void EnemyWalk::UpdateMotion() {
	walkTimer_ += 1.0f / 60.0f;


	// X軸周りに回転アニメーション
	enemy_->SetRotationX(walkTimer_);
}

EnemyDeath::EnemyDeath(BaseEnemy* enemy) { enemy_ = enemy; }

void EnemyDeath::Initialize() {
	velocty_ = {0.0f, kAscensionSpeed, 0.0f};

	deathTimer_ = 0.0f;

	enemy_->SetRotationX(0.0f);
	enemy_->SetRotationY(0.0f);
	enemy_->SetRotationZ(0.0f);

	// 死亡演出開始時の角度を保存
	deathStartRotationZ_ = enemy_->GetRotationZ();
}

void EnemyDeath::Update() {
	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT:
		break;

	case DeathPhase::ASCENSION:

		Vector4 color = enemy_->GetObjectColorRGBA();

		color.x += 0.01f;
		color.y += 0.01f;
		color.z += 0.01f;
		color.w -= 0.01f;

		color.x = std::clamp(color.x, 0.0f, 10.0f);
		color.y = std::clamp(color.y, 0.0f, 10.0f);
		color.z = std::clamp(color.z, 0.0f, 10.0f);
		color.w = std::clamp(color.w, 0.0f, 1.0f);

		enemy_->SetObjectColorRGBA(color);
		break;
	}
}

void EnemyDeath::UpdateMotion() {

	deathTimer_ += 1.0f / 60.0f;

	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT: {

		// デスタイマーを 0.0f ～ 1.0f に変換
		float t = std::clamp(deathTimer_ / kHeadShotMotionTime, 0.0f, 1.0f);

		// X軸回転で倒れる
		enemy_->SetRotationZ(Lerp(deathStartRotationZ_, kHeadShotMotionAngleEnd, Ease::EaseOutElastic(t)));
		if (deathTimer_ >= kHeadShotMotionTime) {
			deathPhase = DeathPhase::ASCENSION;
			deathTimer_ = 0.0f;
		}
	} break;

	case DeathPhase::ASCENSION:

		enemy_->Move(velocty_);

		if (deathTimer_ >= kAscensionMotionTime) {
			deathTimer_ = 0.0f;
			deathPhase = DeathPhase::HEAD_SHOT;
			enemy_->SetDead(true);
		}
		break;
	}
}

