#include "BoomerangEnemy.h"
#include "MapChipField.h"
#include "Vector3Util.h"
#include "Matrix4x4Util.h"
#include<numbers>
#include<cmath>
#include "Player.h"
#include <algorithm>
#include "Ease.h"
#include"GameScene.h"
#include "ModelManager.h"

using namespace KamataEngine;
AABB BoomerangEnemy::GetBoomerangAABB() const { 
	AABB re{};
	Vector3 pos = worldTransformBoomerang_.translation_;

	re.min = {pos.x - kBoomerangWidth / 2.0f, pos.y - kBoomerangHeight / 2.0f, pos.z - kBoomerangWidth / 2.0f};
	re.max = {pos.x + kBoomerangWidth / 2.0f, pos.y + kBoomerangHeight / 2.0f, pos.z + kBoomerangWidth / 2.0f};
	
	return AABB(); }
bool BoomerangEnemy::GetIsBoomerangFlying() { return isBoomerangFlying_; }
void BoomerangEnemy::Draw() {

	WorldTransformUpdate(worldTransformBoomerang_);

	Model::PreDraw();

	model_->Draw(worldTransform_, *camera_);
	modelBoomerang_->Draw(worldTransformBoomerang_, *camera_);

	Model::PostDraw();

}
void BoomerangEnemy::InitializeInternal() {
	model_ = ModelManager::GetInstance()->Load("ThrowingBorn", true);
	modelBoomerang_ = ModelManager::GetInstance()->Load("BoomerangEnemy", true);

	worldTransformBoomerang_.Initialize();

	worldTransformBoomerang_.translation_ = worldTransform_.translation_;

	// モデルの向きに合わせてY軸回りに270度回転させる
	worldTransform_.rotation_.y = kDefaultAngle;
	worldTransformBoomerang_.rotation_.y = kDefaultAngle;

	behaviorRequest_ = EnemyBoomerangBehavior::kWalk;
	ChangeBehavior();

	SetBoomerangFlying(false);
}

void BoomerangEnemy::ChangeBehavior() {

	// 振る舞い変更のリクエストが来ていた場合、振る舞いを変更する
	if (behaviorRequest_ != EnemyBoomerangBehavior::kUnknown) {
		// 振る舞い変更の反映
		enumBehavior_ = behaviorRequest_;

		// 振る舞いごとの初期化
		switch (enumBehavior_) {
			// 例外処理対策
		case EnemyBoomerangBehavior::kUnknown:
		default:
			enumBehavior_ = EnemyBoomerangBehavior::kWalk;

			[[fallthrough]];
		case EnemyBoomerangBehavior::kWalk:
			behavior_ = std::make_unique<EnemyBoomerangWalk>(this);
			break;

		case EnemyBoomerangBehavior::kAttack:
			behavior_ = std::make_unique<EnemyBoomerangAttack>(this);
			break;

		case EnemyBoomerangBehavior::kDeathDirection:
			behavior_ = std::make_unique<EnemyBoomerangDeath>(this);
			break;
		}

		behavior_->Initialize();
		behaviorRequest_ = EnemyBoomerangBehavior::kUnknown;
	}
}

void BoomerangEnemy::OnCollision() {
	// 死んでいるなら何もしない
	if (enumBehavior_ == EnemyBoomerangBehavior::kDeathDirection) {
		return;
	}
}

void BoomerangEnemy::OnCollisionBullet(int bulletAttack) {
	// ダメージを受ける
	HP_ -= bulletAttack;

	// エフェクトを生成
	if (gameScene_) {
		gameScene_->CreateHitEffect(GetWorldPosition());
	}

	// 死んでなければ処理終了
	if (HP_ > 0) {
		return;
	}

	// 振る舞いをデス演出に変更
	behaviorRequest_ = EnemyBoomerangBehavior::kDeathDirection;

	// あたり判定無効フラグを立てる
	isCollisionDisabled_ = true;
}
