#include "EnemyBehavior.h"
#include "Enemy.h"
#include "RotateEnemy.h"
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


EnemyRotateWalk::EnemyRotateWalk(RotateEnemy* rotateEnemy) { rotateEnemy_ = rotateEnemy; }

void EnemyRotateWalk::Initialize() {
	velocty_ = {-kWalkSpeed, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
	walkMotionTimer_ = 0.0f;
}

void EnemyRotateWalk::Update() { 

	walkTimer_ += 1.0f / 60.0f;

	if (walkTimer_ >= kWalkTime) {
		rotateEnemy_->BehaviorRequestFlyRing();
	}

	rotateEnemy_->Move(velocty_); 
		ImGui::Text("%f,%f,%f", rotateEnemy_->GetWorldPosition().x, rotateEnemy_->GetWorldPosition().y, rotateEnemy_->GetWorldPosition().z);
}

void EnemyRotateWalk::UpdateMotion() {
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
	rotateEnemy_->SetRotationX(angle);
}


EnemyRotateFlyRing::EnemyRotateFlyRing(RotateEnemy* rotateEnemy) { rotateEnemy_ = rotateEnemy; }

void EnemyRotateFlyRing::Initialize() { 
	
	//円の中心点
	circleCenterPoint = rotateEnemy_->GetWorldPosition();
	circleCenterPoint.y += kRadius;

	//初期化
	translation_ = rotateEnemy_->GetTranslation();
	angul = -std::numbers::pi_v<float> / 2.0f;
	rotateTimer_ = 0.0f;
}

void EnemyRotateFlyRing::Update() { 
	rotateTimer_ += 1.0f / 60.0f;

	angul -= std::numbers::pi_v<float> * 2.0f / (60.0f * kRotateTime);

	translation_.x = circleCenterPoint.x + std::cos(angul) * kRadius;
	translation_.y = circleCenterPoint.y + std::sin(angul) * kRadius;
	
	//一定以上時間が経ったなら歩行に変更
	if (rotateTimer_ >= kRotateTime) {
		rotateEnemy_->BehaviorRequestWalk();
	}

	rotateEnemy_->GetWorldPosition();


	//円周をまわる
	rotateEnemy_->SetTranslation(translation_);
	ImGui::Text("%f,%f,%f", rotateEnemy_->GetWorldPosition().x, rotateEnemy_->GetWorldPosition().y,rotateEnemy_->GetWorldPosition().z);
}

void EnemyRotateFlyRing::UpdateMotion() {}

EnemyRotateDeath::EnemyRotateDeath(RotateEnemy* rotateenemy) { rotateEnemy_ = rotateenemy; }

void EnemyRotateDeath::Initialize() {

	deathTimer_ = 0.0f;

	// 死亡演出開始時の角度を保存
	deathStartRotationX_ = rotateEnemy_->GetRotationX();
}

void EnemyRotateDeath::Update() {}

void EnemyRotateDeath::UpdateMotion() {

	deathTimer_ += 1.0f / 60.0f;

	// デスタイマーを 0.0f ～ 1.0f に変換
	float t = std::clamp(deathTimer_ / kDeathMotionTime, 0.0f, 1.0f);

	// X軸回転で倒れる
	rotateEnemy_->SetRotationX(Lerp(deathStartRotationX_, kDeathMotionAngleEnd, Ease::EaseInOut(t)));

	// Y軸回転でぐるぐる回る
	rotateEnemy_->SetRotationY(rotateEnemy_->GetRotationY() + kDeathSpinSpeed * (1.0f / 60.0f));

	if (deathTimer_ >= kDeathMotionTime) {
		rotateEnemy_->SetDead(true);
	}
}
