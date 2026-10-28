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

void EnemyDeath::Update() {}

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

