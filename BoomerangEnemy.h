#pragma once
#include<KamataEngine.h>
#include "BaseEnemy.h"
#include"Collision.h"
#include "BoomerangEnemyBehavior.h"


// 前方宣言
class MapChipField;

class BoomerangEnemy final : public BaseEnemy {
public:

	void OnCollision() override;
	void OnCollisionBullet(int bulletAttack) override;

	//振る舞いをリクエストする
	void BehaviorRequestWalk() { behaviorRequest_ = EnemyBoomerangBehavior::kWalk; };
	void BehaviorRequestAttack() { behaviorRequest_ = EnemyBoomerangBehavior::kAttack; };


	//以下ブーメラン用関数
	void MoveBoomerang(const KamataEngine::Vector3& velocity) { worldTransformBoomerang_.translation_ += velocity; };

	// XYZ軸回転設定
	void SetBoomerangRotationX(float angle) { worldTransformBoomerang_.rotation_.x = angle; }
	void SetBoomerangRotationY(float angle) { worldTransformBoomerang_.rotation_.y = angle; }
	void SetBoomerangRotationZ(float angle) { worldTransformBoomerang_.rotation_.z = angle; }

	// XYZ軸回転取得
	float GetBoomerangRotationX() const { return worldTransformBoomerang_.rotation_.x; }
	float GetBoomerangRotationY() const { return worldTransformBoomerang_.rotation_.y; }
	float GetBoomerangRotationZ() const { return worldTransformBoomerang_.rotation_.z; }
	void SetBoomerangTranslation(KamataEngine::Vector3 translation) { worldTransformBoomerang_.translation_ = translation; };

	//プレイヤーとブーメランの当たり判定用
	AABB GetBoomerangAABB() const;
	bool GetIsBoomerangFlying();
	void SetBoomerangFlying(bool isFrying) { isBoomerangFlying_ = isFrying; };


private:

	void Draw() override;

	// 初期化の個別処理
	void InitializeInternal() override;

	// 振る舞いの切り替え
	void ChangeBehavior() override;



	EnemyBoomerangBehavior enumBehavior_ = EnemyBoomerangBehavior::kWalk;
	EnemyBoomerangBehavior behaviorRequest_ = EnemyBoomerangBehavior::kUnknown;

	// モデルの向きに合わせたY軸の回転量(270度)
	static inline const float kDefaultAngle = 3.0f * (kPi / 2.0f);


	//ブーメラン
	KamataEngine::Model* modelBoomerang_ = nullptr;
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransformBoomerang_;
	
	//ブーメランが投げられているか
	bool isBoomerangFlying_ = false;
	
	//ブーメラン当たり判定
	static inline const float kBoomerangWidth = 1.0f;
	static inline const float kBoomerangHeight = 1.0f;

};
