#define NOMINMAX
#include "JumpEnemy.h"
#include "Ease.h"
#include "GameScene.h"
#include "MapChipField.h"
#include "Matrix4x4Util.h"
#include "ModelManager.h"
#include "Player.h"
#include "Vector3Util.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include"SoundManager.h"

using namespace KamataEngine;
void JumpEnemy::InitializeInternal() {
	model_ = ModelManager::GetInstance()->Load("JumpEnemy", true);

	// モデルの向きに合わせてY軸回りに270度回転させる
	worldTransform_.rotation_.y = kDefaultAngle;

	behaviorRequest_ = EnemyJumpBehavior::kJumpWalk;
	ChangeBehavior();
}

void JumpEnemy::ChangeBehavior() {

	// 振る舞い変更のリクエストが来ていた場合、振る舞いを変更する
	if (behaviorRequest_ != EnemyJumpBehavior::kUnknown) {
		// 振る舞い変更の反映
		enumBehavior_ = behaviorRequest_;

		// 振る舞いごとの初期化
		switch (enumBehavior_) {
			// 例外処理対策
		case EnemyJumpBehavior::kUnknown:
		default:
			enumBehavior_ = EnemyJumpBehavior::kJumpWalk;

		case EnemyJumpBehavior::kJumpWalk:
			behavior_ = std::make_unique<EnemyJumpWalk>(this);
			break;

		case EnemyJumpBehavior::kHighJump:
			behavior_ = std::make_unique<EnemyHighJump>(this);
			break;

		case EnemyJumpBehavior::kDeathDirection:
			behavior_ = std::make_unique<EnemyJumpDeath>(this);
			break;
		}
		 
		behavior_->Initialize();
		behaviorRequest_ = EnemyJumpBehavior::kUnknown;
	}
}
	
void JumpEnemy::ResolveMapChipCollision(const KamataEngine::Vector3& velocity) { 

	// 衝突判定の初期化
	CoolisionMapInfo collisionMapInfo{};

	// 移動量のコピー
	collisionMapInfo.moveDistance = velocity;

	MapChipCollisionBottom(collisionMapInfo);

	ApplyCollisionResult(collisionMapInfo);

	isGround_ = collisionMapInfo.onGround;

}

void JumpEnemy::MapChipCollisionBottom(CoolisionMapInfo& info) {
	// 下降あり？
	if (info.moveDistance.y >= 0.0f) {
		return;
	}

	
	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveDistance, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真下の当たり判定を行う
	bool isHit = false;
	IndexSet indexSet{};
	IndexSet hitIndexSet{};

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);

	// 判定するx場所を固定
	indexSet.xIndex = 0;

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);

	// 判定するx場所を固定
	indexSet.xIndex = 0;

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	if (isHit) {
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLeftBottom));

		// 移動前と移動後でY方向のセル番号が変わった場合だけ、地面として処理する
		if (indexSetNow.yIndex != hitIndexSet.yIndex) {
			Rect rect = mapChipField_->RectByIndex(hitIndexSet.xIndex, hitIndexSet.yIndex);

			info.moveDistance.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kHeight / 2.0f + kBlank);

			info.onGround = true;
		}
	}
}

void JumpEnemy::ApplyCollisionResult(const CoolisionMapInfo& info) { worldTransform_.translation_ += info.moveDistance; }

KamataEngine::Vector3 JumpEnemy::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	Vector3 offsetTable[static_cast<uint32_t>(Corner::kNumCorner)] = {
	    Vector3(kWidth / 2.0f, -kHeight / 2.0f, 0.0f),  // 右下
	    Vector3(-kWidth / 2.0f, -kHeight / 2.0f, 0.0f), // 左下
	    Vector3(kWidth / 2.0f, kHeight / 2.0f, 0.0f),   // 右上
	    Vector3(-kWidth / 2.0f, kHeight / 2.0f, 0.0f)   // 左上
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void JumpEnemy::OnCollision() {
	// 死んでいるなら何もしない
	if (enumBehavior_ == EnemyJumpBehavior::kDeathDirection) {
		return;
	}
}

void JumpEnemy::OnCollisionBullet(int bulletAttack) {
	// ダメージを受ける
	HP_ -= bulletAttack;

	// エフェクトを生成
	if (gameScene_) {
		gameScene_->CreateHitEffect(GetWorldPosition());
	}

	// 死んでなければ処理終了
	if (HP_ > 0) {
		// 死んでいなければ通常被弾SEを鳴らす
		SoundManager::GetInstance()->PlaySe(SeType::kEnemyDamage);

		return;
	}

	// 死んでいれば死亡を鳴らす
	SoundManager::GetInstance()->PlaySe(SeType::kEnemyDeath);

	// 振る舞いをデス演出に変更
	behaviorRequest_ = EnemyJumpBehavior::kDeathDirection;

	// あたり判定無効フラグを立てる
	isCollisionDisabled_ = true;
}
