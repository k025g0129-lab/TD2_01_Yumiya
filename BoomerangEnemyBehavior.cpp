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
}

void EnemyBoomerangWalk::Update() {

	walkTimer_ += 1.0f / 60.0f;

	if (walkTimer_ >= kWalkTime) {
		boomerangEnemy_->BehaviorRequestAttack();
	}

	boomerangEnemy_->Move(velocty_);
	
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
	boomerangEnemy_->SetRotationX(angle);
}

EnemyBoomerangAttack::EnemyBoomerangAttack(BoomerangEnemy* boomerangEnemy) { boomerangEnemy_ = boomerangEnemy; }

void EnemyBoomerangAttack::Initialize() {

	// 円の中心点
	verticalCircleCenterPoint = boomerangEnemy_->GetWorldPosition();
	distanceCircleCenterPoint = boomerangEnemy_->GetWorldPosition();
	distanceCircleCenterPoint.x = 2.5f;

	// 初期化
	translation_ = boomerangEnemy_->GetTranslation();
	angul = std::numbers::pi_v<float> / 2.0f;
	attackTimer_ = 0.0f;
	distanceRadius = boomerangEnemy_->GetWorldPosition().x - distanceCircleCenterPoint.x;
}

void EnemyBoomerangAttack::Update() {
	attackTimer_ += 1.0f / 60.0f;

	angul -= std::numbers::pi_v<float> * 2.0f / (60.0f * kRotateTime);

	translation_.x = distanceCircleCenterPoint.x + std::sin(angul) * distanceRadius;
	translation_.y = verticalCircleCenterPoint.y + std::cos(angul) * kVerticalRadius;

	// 一定以上時間が経ったなら歩行に変更
	if (attackTimer_ >= kRotateTime) {
		boomerangEnemy_->BehaviorRequestWalk();
	}

	boomerangEnemy_->GetWorldPosition();

	// 円周をまわる
	boomerangEnemy_->SetTranslation(translation_);
	ImGui::Text("%f,%f,%f", boomerangEnemy_->GetWorldPosition().x, boomerangEnemy_->GetWorldPosition().y, boomerangEnemy_->GetWorldPosition().z);
}

void EnemyBoomerangAttack::UpdateMotion() {}

EnemyBoomerangDeath::EnemyBoomerangDeath(BoomerangEnemy* boomerangEnemy) { boomerangEnemy_ = boomerangEnemy; }

void EnemyBoomerangDeath::Initialize() {

	deathTimer_ = 0.0f;

	// 死亡演出開始時の角度を保存
	deathStartRotationX_ = boomerangEnemy_->GetRotationX();
}

void EnemyBoomerangDeath::Update() {}

void EnemyBoomerangDeath::UpdateMotion() {

	deathTimer_ += 1.0f / 60.0f;

	// デスタイマーを 0.0f ～ 1.0f に変換
	float t = std::clamp(deathTimer_ / kDeathMotionTime, 0.0f, 1.0f);

	// X軸回転で倒れる
	boomerangEnemy_->SetRotationX(Lerp(deathStartRotationX_, kDeathMotionAngleEnd, Ease::EaseInOut(t)));

	// Y軸回転でぐるぐる回る
	boomerangEnemy_->SetRotationY(boomerangEnemy_->GetRotationY() + kDeathSpinSpeed * (1.0f / 60.0f));

	if (deathTimer_ >= kDeathMotionTime) {
		boomerangEnemy_->SetDead(true);
	}
}
