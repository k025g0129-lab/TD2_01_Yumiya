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
	isJump_ = false;
	preIsGround_ = true;
	jumpEnemy_->SetScaleX(1.0f);
	jumpEnemy_->SetScaleY(1.0f);
	jumpEnemy_->SetScaleZ(1.0f);
}

void EnemyJumpWalk::Update() {

	//地上
	if (jumpEnemy_->IsGround()) {
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
	if (jumpEnemy_->IsGround() && !preIsGround_) {

		jumpCount_++;
		
		velocty_.y = 0.0f;

		if (jumpCount_ >= kJumpCountMax) {
			jumpEnemy_->BehaviorRequestHighJump();
			return;
		}

		isJump_ = true;
	} else {

		isJump_ = false;
	}


	preIsGround_ = nowGrounfd;

}

void EnemyJumpWalk::UpdateMotion() {

	jumpEnemy_->SetScaleY(GraduallyChangingScale(jumpEnemy_->GetScaleY(),1.0f,0.015f));
	

	//if (jumpEnemy_->GetScaleY() < 1.0f) {
	//	jumpEnemy_->SetScaleY(jumpEnemy_->GetScaleY() + 0.015f);
	//}

	jumpEnemy_->SetScaleZ(GraduallyChangingScale(jumpEnemy_->GetScaleZ(), 1.0f, 0.015f));
	

	//if (jumpEnemy_->GetScaleZ() > 1.0f) {
	//	jumpEnemy_->SetScaleZ(jumpEnemy_->GetScaleZ() - 0.015f);
	//}

	//着地モーション
	if (isJump_) {
		jumpEnemy_->SetScaleY(0.7f);
		jumpEnemy_->SetScaleZ(1.2f);

	}

}


EnemyHighJump::EnemyHighJump(JumpEnemy* jumpEnemy) { jumpEnemy_ = jumpEnemy; }

void EnemyHighJump::Initialize() {

	highJumpPhase = HighJumpPhase::kCharge;
	chargeTimer_ = 0.0f;
	mapChipField_ = jumpEnemy_->GetMapChipField(); 

	jumpEnemy_->SetScaleX(1.0f);
	jumpEnemy_->SetScaleY(1.0f);
	jumpEnemy_->SetScaleZ(1.0f);
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

		jumpEnemy_->SetScaleY(GraduallyChangingScale(jumpEnemy_->GetScaleY(), 0.5f, 0.03f));
	

		//if (jumpEnemy_->GetScaleY() > 0.5f) {
		//	jumpEnemy_->SetScaleY(jumpEnemy_->GetScaleY() - 0.03f);
		//} else {
		//	jumpEnemy_->SetScaleY(0.5f);
		//}

		jumpEnemy_->SetScaleZ(GraduallyChangingScale(jumpEnemy_->GetScaleZ(), 1.5f, 0.03f));
	

		//if (jumpEnemy_->GetScaleZ() < 1.5f) {
		//	jumpEnemy_->SetScaleZ(jumpEnemy_->GetScaleZ() + 0.03f);
		//} else {
		//	jumpEnemy_->SetScaleZ(1.5f);
		//}

		break;

	case HighJumpPhase::kJump:

		jumpEnemy_->SetScaleY(GraduallyChangingScale(jumpEnemy_->GetScaleY(), 1.1f, 0.03f));
	

		//if (jumpEnemy_->GetScaleY() < 1.1f) {
		//	jumpEnemy_->SetScaleY(jumpEnemy_->GetScaleY() + 0.03f);
		//} else {
		//	jumpEnemy_->SetScaleY(1.1f);
		//}

		jumpEnemy_->SetScaleZ(GraduallyChangingScale(jumpEnemy_->GetScaleZ(), 0.9f, 0.03f));
	

		//if (jumpEnemy_->GetScaleZ() < 0.9f) {
		//	jumpEnemy_->SetScaleZ(jumpEnemy_->GetScaleZ() - 0.03f);
		//} else {
		//	jumpEnemy_->SetScaleZ(0.9f);
		//}
		break;
	}

}


EnemyJumpDeath::EnemyJumpDeath(JumpEnemy* jumpEnemy) { jumpEnemy_ = jumpEnemy; }

void EnemyJumpDeath::Initialize() {
	velocty_ = {0.0f, kAscensionSpeed, 0.0f};

	deathTimer_ = 0.0f;

	mapChipField_ = jumpEnemy_->GetMapChipField();

	jumpEnemy_->SetRotationX(0.0f);
	jumpEnemy_->SetRotationY(3.0f * (kPi / 2.0f));
	jumpEnemy_->SetRotationZ(0.0f);

	// 死亡演出開始時の角度を保存
	deathStartRotationZ_ = jumpEnemy_->GetRotationZ();
}

void EnemyJumpDeath::Update() {
	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT:
		break;

	case DeathPhase::ASCENSION:

		Vector4 color = jumpEnemy_->GetObjectColorRGBA();

		color.x += 0.01f;
		color.y += 0.01f;
		color.z += 0.01f;
		color.w -= 0.01f;

		color.x = std::clamp(color.x, 0.0f, 10.0f);
		color.y = std::clamp(color.y, 0.0f, 10.0f);
		color.z = std::clamp(color.z, 0.0f, 10.0f);
		color.w = std::clamp(color.w, 0.0f, 1.0f);

		jumpEnemy_->SetObjectColorRGBA(color);
		break;
	}
}

void EnemyJumpDeath::UpdateMotion() {

	
	deathTimer_ += 1.0f / 60.0f;

	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT: {

		// デスタイマーを 0.0f ～ 1.0f に変換
		float t = std::clamp(deathTimer_ / kHeadShotMotionTime, 0.0f, 1.0f);

		// X軸回転で倒れる
		jumpEnemy_->SetRotationZ(Lerp(deathStartRotationZ_, kHeadShotMotionAngleEnd, Ease::EaseOutElastic(t)));
		if (deathTimer_ >= kHeadShotMotionTime) {
			deathPhase = DeathPhase::ASCENSION;
			deathTimer_ = 0.0f;
		}
	} break;

	case DeathPhase::ASCENSION:

		jumpEnemy_->Move(velocty_);

		if (deathTimer_ >= kAscensionMotionTime) {
			deathTimer_ = 0.0f;
			deathPhase = DeathPhase::HEAD_SHOT;
			jumpEnemy_->SetDead(true);
		}
		break;
	}
}

float GraduallyChangingScale(float self, float target, float change) {

	
	//目標近くの時
	if (self <= target + change && self >= target - change) {

		//シェイク用で大きく震わせる
		return self + (change * 3.0f) ;

	//目標の近くではない時
	} else {
	
		//目標に近づけさせる
		if (self > target ) {
			return self - change;
		
		} else if(self < target) {
			return self + change;
		
		}
	}

	return self;
}

void ShakeScale(float& self, float shake1, float shake2) { 
	if (self == shake1) {
		self = shake1;
	
	} else if (self == shake2) {
		self = shake2;
	
	} else {
		self = shake1;
	
	}


}
