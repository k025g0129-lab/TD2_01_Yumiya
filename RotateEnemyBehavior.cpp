#include "RotateEnemyBehavior.h"
#include "Enemy.h"
#include "RotateEnemy.h"
#include "Matrix4x4Util.h"
#include <algorithm>
#include "Ease.h"

using namespace KamataEngine;


EnemyRotateWalk::EnemyRotateWalk(RotateEnemy* rotateEnemy) { rotateEnemy_ = rotateEnemy; }


void EnemyRotateWalk::Initialize() {
	velocty_ = {-kWalkSpeed, 0.0f, 0.0f};

	walkTimer_ = 0.0f;
	walkMotionTimer_ = 0.0f;
	rotateEnemy_->SetRotationX(0.0f);
	rotateEnemy_->SetRotationY(0.0f);
	rotateEnemy_->SetRotationZ(0.0f);
}

void EnemyRotateWalk::Update() {

	walkTimer_ += 1.0f / 60.0f;

	if (walkTimer_ >= kWalkTime) {
		rotateEnemy_->BehaviorRequestFlyRing();
	}

	rotateEnemy_->Move(velocty_);
	//ImGui::Text("%f,%f,%f", rotateEnemy_->GetWorldPosition().x, rotateEnemy_->GetWorldPosition().y, rotateEnemy_->GetWorldPosition().z);
}

void EnemyRotateWalk::UpdateMotion() {

	// X軸周りに回転アニメーション
	rotateEnemy_->SetRotationX(rotateEnemy_->GetRotationX() + 0.01f);
}

EnemyRotateFlyRing::EnemyRotateFlyRing(RotateEnemy* rotateEnemy) { rotateEnemy_ = rotateEnemy; }

void EnemyRotateFlyRing::Initialize() {

	// 円の中心点
	circleCenterPoint = rotateEnemy_->GetWorldPosition();
	circleCenterPoint.y += kRadius;

	// 初期化
	translation_ = rotateEnemy_->GetTranslation();
	angul = -std::numbers::pi_v<float> / 2.0f;
	rotateTimer_ = 0.0f;
}

void EnemyRotateFlyRing::Update() {
	rotateTimer_ += 1.0f / 60.0f;

	angul -= std::numbers::pi_v<float> * 2.0f / (60.0f * kRotateTime);

	translation_.x = circleCenterPoint.x + std::cos(angul) * kRadius;
	translation_.y = circleCenterPoint.y + std::sin(angul) * kRadius;

	// 一定以上時間が経ったなら歩行に変更
	if (rotateTimer_ >= kRotateTime) {
		rotateEnemy_->BehaviorRequestWalk();
	}

	rotateEnemy_->GetWorldPosition();
		
	// 円周をまわる
	rotateEnemy_->SetTranslation(translation_);
	ImGui::Text("%f,%f,%f", rotateEnemy_->GetWorldPosition().x, rotateEnemy_->GetWorldPosition().y, rotateEnemy_->GetWorldPosition().z);
}

void EnemyRotateFlyRing::UpdateMotion() {



	// Z軸周りに回転アニメーション
	rotateEnemy_->SetRotationZ(rotateEnemy_->GetRotationZ() - (std::numbers::pi_v<float> * 2.0f) / (60.0f * kRotateTime));

	// X軸周りに回転アニメーション
	rotateEnemy_->SetRotationX(rotateEnemy_->GetRotationX() + 0.01f);

}

EnemyRotateDeath::EnemyRotateDeath(RotateEnemy* rotateenemy) { rotateEnemy_ = rotateenemy; }

void EnemyRotateDeath::Initialize() {
	velocty_ = {0.0f, kAscensionSpeed, 0.0f};

	deathTimer_ = 0.0f;


	rotateEnemy_->SetRotationX(0.0f);
	rotateEnemy_->SetRotationY(0.0f);
	rotateEnemy_->SetRotationZ(0.0f);

	// 死亡演出開始時の角度を保存
	deathStartRotationZ_ = rotateEnemy_->GetRotationZ();


}

void EnemyRotateDeath::Update() {
	switch (deathPhase) {

	case DeathPhase::HEAD_SHOT: 
		break;

	case DeathPhase::ASCENSION:

		Vector4 color = rotateEnemy_->GetObjectColorRGBA();

		color.x += 0.01f;
		color.y += 0.01f;
		color.z += 0.01f;
		color.w -= 0.01f;

		color.x = std::clamp(color.x, 0.0f, 10.0f);
		color.y = std::clamp(color.y, 0.0f, 10.0f);
		color.z = std::clamp(color.z, 0.0f, 10.0f);
		color.w = std::clamp(color.w, 0.0f, 1.0f);

		rotateEnemy_->SetObjectColorRGBA(color);
		break;
	
	}



}

void EnemyRotateDeath::UpdateMotion() {

	deathTimer_ += 1.0f / 60.0f;

	switch (deathPhase) {

		case DeathPhase::HEAD_SHOT: {

			// デスタイマーを 0.0f ～ 1.0f に変換
			float t = std::clamp(deathTimer_ / kHeadShotMotionTime, 0.0f, 1.0f);

			// X軸回転で倒れる
			rotateEnemy_->SetRotationZ(Lerp(deathStartRotationZ_, kHeadShotMotionAngleEnd, Ease::EaseOutElastic(t)));
			if (deathTimer_ >= kHeadShotMotionTime) {
				deathPhase = DeathPhase::ASCENSION;
				deathTimer_ = 0.0f;
			}
		}
		break;

		case DeathPhase::ASCENSION:

			rotateEnemy_->Move(velocty_);			

			if (deathTimer_ >= kAscensionMotionTime) {
			    deathTimer_ = 0.0f;
			    deathPhase = DeathPhase::HEAD_SHOT;
				rotateEnemy_->SetDead(true);
			}
		break;

	}

}
