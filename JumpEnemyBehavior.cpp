#include "JumpEnemyBehavior.h"
#include "Enemy.h"
#include "JumpEnemy.h"
#include "Matrix4x4Util.h"
#include <algorithm>
#include "Ease.h"

using namespace KamataEngine;

EnemyJumpWalk::EnemyJumpWalk(JumpEnemy* jumpEnemy) { jumpEnemy_ = jumpEnemy; }

void EnemyJumpWalk::Initialize() {

	velocty_ = {-kWalkSpeed, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
	walkMotionTimer_ = 0.0f;
	mapChipField_ = jumpEnemy_->GetMapChipField();

	jumpCount_ = 0;
	isGround_ = false;
	preIsGround_ = true;
}

void EnemyJumpWalk::Update() {

	isGround_ = jumpEnemy_->IsGround();


	//地上
	if (isGround_) {
		velocty_.y += kJumpPower;

		jumpEnemy_->SetGround(false);
	}

	//空中
	if (!jumpEnemy_->IsGround()) {
		velocty_.y -= kGravity;
	}

	jumpEnemy_->ResolveMovement(velocty_);

	bool nowGrounfd = jumpEnemy_->IsGround();
		
	// 着地
	if (nowGrounfd && !preIsGround_) {
		jumpCount_++;
		
		velocty_.y = 0.0f;

		if (jumpCount_ >= kJumpCountMax) {
			jumpEnemy_->BehaviorRequestHighJump();
			return;
		}
	}

	preIsGround_ = nowGrounfd;

}

void EnemyJumpWalk::UpdateMotion() {

	walkMotionTimer_ += 1.0f / 60.0f;

	// タイマーが大きくなりすぎないようにする
	if (walkMotionTimer_ >= kWalkMotionTime) {
		walkMotionTimer_ -= kWalkMotionTime;
	}

	// -1.0f ～ 1.0f を周期的に繰り返す値
	float param = std::sin(2.0f * kPi * walkMotionTimer_ / kWalkMotionTime);

	// -1.0f ～ 1.0f を 0.0f ～ 1.0f に変換
	float t = (param + 1.0f) / 2.0f;

	// 最初の角度から最後の角度まで補間
	float angle = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * t;

	// X軸周りに回転アニメーション
	jumpEnemy_->SetRotationX(angle);
}


EnemyHighJump::EnemyHighJump(JumpEnemy* jumpEnemy) { jumpEnemy_ = jumpEnemy; }

void EnemyHighJump::Initialize() {

	highJumpPhase = HighJumpPhase::kCharge;
	chargeTimer_ = 0.0f;
	mapChipField_ = jumpEnemy_->GetMapChipField(); 
}

void EnemyHighJump::Update() {

	switch (highJumpPhase) {
		
	case HighJumpPhase::kCharge:
		chargeTimer_++;

		if (chargeTimer_ >= kChargeTime) {
			highJumpPhase = HighJumpPhase::kJump;
			velocity_.y = kHighJumpPower;
			jumpEnemy_->SetGround(false);
		}

		break;

	case HighJumpPhase::kJump:
		velocity_.y -= kGravity;

		jumpEnemy_->ResolveMovement(velocity_);

		if (jumpEnemy_->IsGround()) {

			velocity_ = {};

			jumpEnemy_->BehaviorRequestJumpWalk();
		}

		break;

	}

	


}

void EnemyHighJump::UpdateMotion() {
	switch (highJumpPhase) {

	case HighJumpPhase::kCharge:

		break;

	case HighJumpPhase::kJump:

		break;
	}

}


EnemyJumpDeath::EnemyJumpDeath(JumpEnemy* jumpEnemy) { jumpEnemy_ = jumpEnemy; }

void EnemyJumpDeath::Initialize() {
	deathTimer_ = 0.0f;

	// 死亡演出開始時の角度を保存
	deathStartRotationX_ = jumpEnemy_->GetRotationX();
	mapChipField_ = jumpEnemy_->GetMapChipField();
}

void EnemyJumpDeath::Update() {}

void EnemyJumpDeath::UpdateMotion() {

	deathTimer_ += 1.0f / 60.0f;

	// デスタイマーを 0.0f ～ 1.0f に変換
	float t = std::clamp(deathTimer_ / kDeathMotionTime, 0.0f, 1.0f);

	// X軸回転で倒れる
	jumpEnemy_->SetRotationX(Lerp(deathStartRotationX_, kDeathMotionAngleEnd, Ease::EaseInOut(t)));

	// Y軸回転でぐるぐる回る
	jumpEnemy_->SetRotationY(jumpEnemy_->GetRotationY() + kDeathSpinSpeed * (1.0f / 60.0f));

	if (deathTimer_ >= kDeathMotionTime) {
		jumpEnemy_->SetDead(true);
	}

}
