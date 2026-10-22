#include "Enemy.h"
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

void Enemy::InitializeInternal() {
	model_ = ModelManager::GetInstance()->Load("enemy", true);

	// モデルの向きに合わせてY軸回りに270度回転させる
	worldTransform_.rotation_.y = kDefaultAngle;

	behaviorRequest_ = EnemyBehavior::kWalk;
	ChangeBehavior();
}

void Enemy::ChangeBehavior() {

	// 振る舞い変更のリクエストが来ていた場合、振る舞いを変更する
	if (behaviorRequest_ != EnemyBehavior::kUnknown) {
		// 振る舞い変更の反映
		enumBehavior_ = behaviorRequest_;

		// 振る舞いごとの初期化
		switch (enumBehavior_) {
			// 例外処理対策
		case EnemyBehavior::kUnknown:
		default:
			enumBehavior_ = EnemyBehavior::kWalk;
			[[fallthrough]];
		case EnemyBehavior::kWalk:
			behavior_ = std::make_unique<EnemyWalk>(this);
			break;

		case EnemyBehavior::kDeathDirection:
			behavior_ = std::make_unique<EnemyDeath>(this);
			break;
		}

		behavior_->Initialize();
		behaviorRequest_ = EnemyBehavior::kUnknown;
	}
}

void Enemy::OnCollision(Player* player) {
	// 死んでいるなら何もしない
	if (enumBehavior_ == EnemyBehavior::kDeathDirection) {
		return;
	}

	// プレイヤーが攻撃中なら死ぬ
	if (player->IsAttack()) {
		// 振る舞いをデス演出に変更
		behaviorRequest_ = EnemyBehavior::kDeathDirection;

		// あたり判定無効フラグを立てる
		isCollisionDisabled_ = true;

		// 敵と自キャラの中間位置にエフェクトを生成
		Vector3 effectPosition = (GetWorldPosition() + player->GetWorldPosition()) / 2.0f;

		if (gameScene_) {
			gameScene_->CreateHitEffect(effectPosition);
		}
	}
}

void Enemy::OnCollisionBullet() {
	// 振る舞いをデス演出に変更
	behaviorRequest_ = EnemyBehavior::kDeathDirection;

	// あたり判定無効フラグを立てる
	isCollisionDisabled_ = true;

	// エフェクトを生成
	if (gameScene_) {
		gameScene_->CreateHitEffect(GetWorldPosition());
	}
}
