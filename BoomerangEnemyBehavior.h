#pragma once
#include "IEnemyBehavior.h"
#include "Vector3Util.h"
#include <KamataEngine.h>

// 前方宣言
class BaseEnemy;
class BoomerangEnemy;

// 振る舞い
enum class EnemyBoomerangBehavior {

	kWalk,           // 歩行
	kAttack,        // 攻撃
	kDeathDirection, // 死亡演出

	kUnknown
};



class EnemyBoomerangWalk : public IEnemiesBehavior {
public:
	explicit EnemyBoomerangWalk(BoomerangEnemy* boomerangEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	BoomerangEnemy* boomerangEnemy_;

	KamataEngine::Vector3 velocty_{};
	float walkTimer_ = 0.0f;
	float walkMotionTimer_ = 0.0f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.03f;

	// 歩行モーションの最初の角度
	static inline const float kWalkMotionAngleStart = kPi / 4.0f;

	// 歩行モーション終わりの角度
	static inline const float kWalkMotionAngleEnd = -kWalkMotionAngleStart;

	// 歩行アニメーションの周期[秒]
	static inline const float kWalkMotionTime = 2.0f;

	// 回転までの周期[秒]
	static inline const float kWalkTime = 5.0f;
};

class EnemyBoomerangAttack : public IEnemiesBehavior {
public:
	explicit EnemyBoomerangAttack(BoomerangEnemy* boomerangEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	BoomerangEnemy* boomerangEnemy_;

	KamataEngine::Vector3 translation_{};
	float attackTimer_ = 0.0f;
	float angul = -std::numbers::pi_v<float>;

	// 縦円の中心点
	KamataEngine::Vector3 verticalCircleCenterPoint = {0.0f, 0.0f, 0.0f};

	//横円の中心点
	KamataEngine::Vector3 distanceCircleCenterPoint = {0.0f, 0.0f, 0.0f};

	// 円の半径
	static inline const float kVerticalRadius = 4.0f;
	float distanceRadius = 2.5f;

	// 何秒で1回転するか
	static inline const float kRotateTime = 5.0f;
};

class EnemyBoomerangDeath : public IEnemiesBehavior {
public:
	explicit EnemyBoomerangDeath(BoomerangEnemy* boomerangEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	BoomerangEnemy* boomerangEnemy_;

	// 死亡演出タイマー
	float deathTimer_ = 0.0f;

	// 死亡演出開始時のX回転
	float deathStartRotationX_ = 0.0f;

	// 死亡演出時間
	static inline const float kDeathMotionTime = 1.0f;

	// 死亡演出で倒れ切る角度
	static inline const float kDeathMotionAngleEnd = -kPi / 2.0f;

	// 死亡演出中の回転速度
	static inline const float kDeathSpinSpeed = kPi * 6.0f;
};