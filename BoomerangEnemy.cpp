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
void BoomerangEnemy::InitializeInternal() {
	model_ = ModelManager::GetInstance()->Load("BoomerangEnemy", true);

	// モデルの向きに合わせてY軸回りに270度回転させる
	worldTransform_.rotation_.y = kDefaultAngle;

	behaviorRequest_ = EnemyBoomerangBehavior::kWalk;
	ChangeBehavior();
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

void BoomerangEnemy::OnCollision(Player* player) {
	// 死んでいるなら何もしない
	if (enumBehavior_ == EnemyBoomerangBehavior::kDeathDirection) {
		return;
	}

	// プレイヤーが攻撃中なら死ぬ
	if (player->IsAttack()) {
		// 振る舞いをデス演出に変更
		behaviorRequest_ = EnemyBoomerangBehavior::kDeathDirection;

		// あたり判定無効フラグを立てる
		isCollisionDisabled_ = true;

		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPosition = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;

		if (gameScene_) {
			gameScene_->CreateHitEffect(effectPosition);
		}
	}
}
