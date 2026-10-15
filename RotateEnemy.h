#pragma once
#include<KamataEngine.h>
#include "BaseEnemy.h"
#include"Collision.h"
#include "EnemyBehavior.h"

// 前方宣言
class MapChipField;
class Player;

class RotateEnemy final : public BaseEnemy {
public:

	void OnCollision(Player* player) override;

	//振る舞いをリクエストする
	void BehaviorRequestWalk() { behaviorRequest_ = EnemyBehavior::kRotateWalk; };
	void BehaviorRequestFlyRing() { behaviorRequest_ = EnemyBehavior::kRotateFlyRing; };

private:

		// 初期化の個別処理
	void InitializeInternal() override;

	// 振る舞いの切り替え
	void ChangeBehavior() override;



	EnemyBehavior enumBehavior_ = EnemyBehavior::kRotateWalk;
	EnemyBehavior behaviorRequest_ = EnemyBehavior::kUnknown;

	// モデルの向きに合わせたY軸の回転量(270度)
	static inline const float kDefaultAngle = 3.0f * (kPi / 2.0f);

};
