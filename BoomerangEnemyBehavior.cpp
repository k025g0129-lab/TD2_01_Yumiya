#include "BoomerangEnemyBehavior.h"
#include "Enemy.h"
#include "BoomerangEnemy.h"
#include "Matrix4x4Util.h"
#include <algorithm>
#include "Ease.h"

using namespace KamataEngine;


EnemyBoomerangWalk::EnemyBoomerangWalk(BoomerangEnemy* boomerangEnemy) { boomerangEnemy_ = boomerangEnemy; }

void EnemyBoomerangWalk::Initialize() {
	velocty_ = {-kWalkSpeed, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
	walkMotionTimer_ = 0.0f;

	//向き調整
	boomerangEnemy_->SetRotationX(0.0f);
	boomerangEnemy_->SetRotationY(3.0f * (kPi / 2.0f));
	boomerangEnemy_->SetRotationZ(0.0f);
	boomerangEnemy_->SetBoomerangRotationX(0.0f);
	boomerangEnemy_->SetBoomerangRotationY(3.0f * (kPi / 2.0f));
	boomerangEnemy_->SetBoomerangRotationZ(0.0f);

	//投げているかどうか
	boomerangEnemy_->SetBoomerangFlying(false);

}

void EnemyBoomerangWalk::Update() {

	walkTimer_ += 1.0f / 60.0f;

	if (walkTimer_ >= kWalkTime) {
		boomerangEnemy_->BehaviorRequestAttack();
	}

	boomerangEnemy_->Move(velocty_);
	boomerangEnemy_->MoveBoomerang(velocty_);
	
}

void EnemyBoomerangWalk::UpdateMotion() {

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
	boomerangEnemy_->SetRotationY(angle + 3.0f * (kPi / 2.0f));
	boomerangEnemy_->SetBoomerangRotationY(boomerangEnemy_->GetRotationY());

}

EnemyBoomerangAttack::EnemyBoomerangAttack(BoomerangEnemy* boomerangEnemy) { boomerangEnemy_ = boomerangEnemy; }

void EnemyBoomerangAttack::Initialize() {

	// 円の中心点
	verticalCircleCenterPoint = boomerangEnemy_->GetWorldPosition();
	distanceCircleCenterPoint = boomerangEnemy_->GetWorldPosition();
	distanceCircleCenterPoint.x = 15.0f;

	// 初期化
	translation_ = boomerangEnemy_->GetTranslation();
	attackTimer_ = 0.0f;
	distanceRadius = boomerangEnemy_->GetWorldPosition().x - distanceCircleCenterPoint.x;

	//モーション
	attackPhase = AttackPhase::CHARGE;
	rotateAngul = 0.0f;
	
}

void EnemyBoomerangAttack::Update() {
	attackTimer_ += 1.0f / 60.0f;

	switch (attackPhase) {

	case AttackPhase::CHARGE:

		if (attackTimer_ >= kChargeTime) {
			attackPhase = AttackPhase::THROWING;
			attackTimer_ = 0.0f;

			//ブーメランを投げる
			boomerangEnemy_->SetBoomerangFlying(true);
		}

		break;

	case AttackPhase::THROWING:
	
		circleAngul -= std::numbers::pi_v<float> * 2.0f / (60.0f * kRotateTime);

		translation_.x = distanceCircleCenterPoint.x + std::sin(circleAngul) * distanceRadius;
		translation_.y = verticalCircleCenterPoint.y + std::cos(circleAngul) * kVerticalRadius;

		// 一定以上時間が経ったなら歩行に変更
		if (attackTimer_ >= kRotateTime) {

			boomerangEnemy_->BehaviorRequestWalk();
			boomerangEnemy_->SetBoomerangFlying(false);
		}

		boomerangEnemy_->GetWorldPosition();

		// 円周をまわる
		boomerangEnemy_->SetBoomerangTranslation(translation_);
		break;
	}

}

void EnemyBoomerangAttack::UpdateMotion() {
	switch (attackPhase) {

	case AttackPhase::CHARGE:
		
		//回転速度
		rotateAngul +=kChargeRotateAcceleration;

		if (rotateAngul >= kChargeRotateMaxSpeed) {
			rotateAngul = kChargeRotateMaxSpeed;
		}

		boomerangEnemy_->SetRotationY(boomerangEnemy_->GetRotationY() - rotateAngul);
		boomerangEnemy_->SetBoomerangRotationY(boomerangEnemy_->GetRotationY());

		break;

	case AttackPhase::THROWING:
		boomerangEnemy_->SetBoomerangRotationY(boomerangEnemy_->GetBoomerangRotationY() - rotateAngul);
		break;
	}

}

EnemyBoomerangDeath::EnemyBoomerangDeath(BoomerangEnemy* boomerangEnemy) { boomerangEnemy_ = boomerangEnemy; }

void EnemyBoomerangDeath::Initialize() {
	velocty_ = {0.0f, kAscensionSpeed, 0.0f};

	deathTimer_ = 0.0f;

	boomerangEnemy_->SetRotationX(0.0f);
	boomerangEnemy_->SetRotationY(3.0f * (kPi / 2.0f));
	boomerangEnemy_->SetRotationZ(0.0f);

	// 死亡演出開始時の角度を保存
	deathStartRotationZ_ = boomerangEnemy_->GetRotationZ();


}

void EnemyBoomerangDeath::Update() {
	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT:
		break;

	case DeathPhase::ASCENSION:

		Vector4 color = boomerangEnemy_->GetObjectColorRGBA();

		color.x += 0.01f;
		color.y += 0.01f;
		color.z += 0.01f;
		color.w -= 0.01f;

		color.x = std::clamp(color.x, 0.0f, 10.0f);
		color.y = std::clamp(color.y, 0.0f, 10.0f);
		color.z = std::clamp(color.z, 0.0f, 10.0f);
		color.w = std::clamp(color.w, 0.0f, 1.0f);

		boomerangEnemy_->SetObjectColorRGBA(color);
		break;
	}
}

void EnemyBoomerangDeath::UpdateMotion() {

	deathTimer_ += 1.0f / 60.0f;

	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT: {

		// デスタイマーを 0.0f ～ 1.0f に変換
		float t = std::clamp(deathTimer_ / kHeadShotMotionTime, 0.0f, 1.0f);

		// X軸回転で倒れる
		boomerangEnemy_->SetRotationZ(Lerp(deathStartRotationZ_, kHeadShotMotionAngleEnd, Ease::EaseOutElastic(t)));
		if (deathTimer_ >= kHeadShotMotionTime) {
			deathPhase = DeathPhase::ASCENSION;
			deathTimer_ = 0.0f;
		}
	} break;

	case DeathPhase::ASCENSION:

		boomerangEnemy_->Move(velocty_);

		if (deathTimer_ >= kAscensionMotionTime) {
			deathTimer_ = 0.0f;
			deathPhase = DeathPhase::HEAD_SHOT;
			boomerangEnemy_->SetDead(true);
		}
		break;
	}
}

float GraduallyChangingScaleA(float self, float target, float change) {

	// 目標近くの時
	if (self <= target + change && self >= target - change) {

		// シェイク用で大きく震わせる
		return self + (change * 3.0f);

		// 目標の近くではない時
	} else {

		// 目標に近づけさせる
		if (self > target) {
			return self - change;

		} else if (self < target) {
			return self + change;
		}
	}

	return self;
}

float RoundTrip(float self, float target1, float target2, float& change, float second) { 

	if (self >= target1) {

		if (change >= 0.0f) {

			change *= -1.0f;
		}

	}

	if (self <= target2) {
		if (change <= 0.0f) {

			change *= -1.0f;
		}
	}



	return self + (change / (second * 60.0f)); 

}
