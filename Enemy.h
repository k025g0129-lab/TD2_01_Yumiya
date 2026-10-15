#pragma once
#include<KamataEngine.h>
#include "BaseEnemy.h"
#include"Collision.h"
#include "EnemyBehavior.h"

// 前方宣言
class MapChipField;
class Player;

class Enemy final : public BaseEnemy {
public:
	void OnCollision(Player *player) override;

private:
	// 初期化の個別処理
	void InitializeInternal() override;

	// 振る舞いの切り替え
	void ChangeBehavior() override;

	EnemyBehavior enumBehavior_ = EnemyBehavior::kWalk;
	EnemyBehavior behaviorRequest_ = EnemyBehavior::kUnknown;

	// モデルの向きに合わせたY軸の回転量(270度)
	static inline const float kDefaultAngle = 3.0f * (kPi / 2.0f);
};
