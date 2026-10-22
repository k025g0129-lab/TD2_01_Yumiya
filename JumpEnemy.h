#pragma once
#include "BaseEnemy.h"
#include "JumpEnemyBehavior.h"
#include "MapChipField.h"
#include "Collision.h"
#include <KamataEngine.h>


// 前方宣言
class Player;

class JumpEnemy final : public BaseEnemy {
public:
	void OnCollision(Player* player) override;

	// 振る舞いをリクエストする
	void BehaviorRequestJumpWalk() { behaviorRequest_ = EnemyJumpBehavior::kJumpWalk; };
	void BehaviorRequestHighJump() { behaviorRequest_ = EnemyJumpBehavior::kHighJump; };
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; };
	MapChipField* GetMapChipField() {return mapChipField_; };

	bool IsGround() const { return isGround_; };
	void SetGround(bool isGround) { isGround_ = isGround; };
	void ResolveMovement(const KamataEngine::Vector3& velocity) { ResolveMapChipCollision(velocity); }


private:
	// 初期化の個別処理
	void InitializeInternal() override;

	// 振る舞いの切り替え
	void ChangeBehavior() override;

	EnemyJumpBehavior enumBehavior_ = EnemyJumpBehavior::kJumpWalk;
	EnemyJumpBehavior behaviorRequest_ = EnemyJumpBehavior::kUnknown;

	// モデルの向きに合わせたY軸の回転量(270度)
	static inline const float kDefaultAngle = 3.0f * (kPi / 2.0f);

	enum Corner { 
		kRightBottom, 
		kLeftBottom, 
		kRightTop, kLeftTop, kNumCorner };

	//当たり判定
	struct CoolisionMapInfo {
		bool isHitCeiling = false;
		bool onGround = false;

		 KamataEngine::Vector3 moveDistance = {};	
	};

	void ResolveMapChipCollision(const KamataEngine::Vector3& velocity);

	//マップチップ(今回は地面のみ)
	void MapChipCollisionBottom(CoolisionMapInfo& info);	

	// 衝突結果を反映して移動させる
	void ApplyCollisionResult(const CoolisionMapInfo& info);

	//角位置
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	bool isGround_ = true;
	MapChipField* mapChipField_ = nullptr;


	static inline const float kGravityAcceleration = 0.05f;
	static inline const float kLimitFallSpeed = 0.8f;
	static inline const float kBlank = 0.05f;

};
