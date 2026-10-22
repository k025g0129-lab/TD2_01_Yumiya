#pragma once
#include<KamataEngine.h>
#include "BaseEnemy.h"
#include"Collision.h"
#include "BoomerangEnemyBehavior.h"


// 前方宣言
class MapChipField;
class Player;

class BoomerangEnemy final : public BaseEnemy {
public:

	void OnCollision(Player* player) override;
	void OnCollisionBullet() override;

	//振る舞いをリクエストする
	void BehaviorRequestWalk() { behaviorRequest_ = EnemyBoomerangBehavior::kWalk; };
	void BehaviorRequestAttack() { behaviorRequest_ = EnemyBoomerangBehavior::kAttack; };

private:

	// 初期化の個別処理
	void InitializeInternal() override;

	// 振る舞いの切り替え
	void ChangeBehavior() override;



	EnemyBoomerangBehavior enumBehavior_ = EnemyBoomerangBehavior::kWalk;
	EnemyBoomerangBehavior behaviorRequest_ = EnemyBoomerangBehavior::kUnknown;

	// モデルの向きに合わせたY軸の回転量(270度)
	static inline const float kDefaultAngle = 3.0f * (kPi / 2.0f);

};
