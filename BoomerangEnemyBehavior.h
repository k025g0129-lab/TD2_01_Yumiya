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


	// 攻撃までの周期[秒]
	static inline const float kWalkTime = 5.0f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	//モーションの周期[秒]
	static inline const float kWalkMotionTime = 2.0f;

	// 歩行モーションの最初の角度
	static inline const float kWalkMotionAngleStart = kPi / 8.0f;

	// 歩行モーション終わりの角度
	static inline const float kWalkMotionAngleEnd = -kWalkMotionAngleStart;

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
	float circleAngul = std::numbers::pi_v<float> / 2.0f;
	float rotateAngul = 0.0f;
	float a = 0.0f;

	// 縦円の中心点
	KamataEngine::Vector3 verticalCircleCenterPoint = {0.0f, 0.0f, 0.0f};

	//横円の中心点
	KamataEngine::Vector3 distanceCircleCenterPoint = {0.0f, 0.0f, 0.0f};

	// 円の半径
	static inline const float kVerticalRadius = 4.0f;
	float distanceRadius = 2.5f;

	// 何秒溜めるか
	static inline const float kChargeTime = 2.0f;

	// 何秒で1回転するか
	static inline const float kRotateTime = 5.0f;


	//攻撃フェーズ
	enum AttackPhase{
		CHARGE,
		THROWING,
	};

	AttackPhase attackPhase = AttackPhase::CHARGE;

	// チャージ時の回転の加速度
	static inline const float kChargeRotateAcceleration = 0.006f;

	// チャージ時の最大回転速度
	static inline const float kChargeRotateMaxSpeed = 0.3f;


};

class EnemyBoomerangDeath : public IEnemiesBehavior {
public:
	explicit EnemyBoomerangDeath(BoomerangEnemy* boomerangEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	BoomerangEnemy* boomerangEnemy_;
	KamataEngine::Vector3 velocty_{};

	enum DeathPhase {
		HEAD_SHOT,
		ASCENSION,
	};

	DeathPhase deathPhase = DeathPhase::HEAD_SHOT;

	// 死亡演出タイマー
	float deathTimer_ = 0.0f;

	// 死亡演出開始時のZ回転
	float deathStartRotationZ_ = 0.0f;

	// 演出時間
	static inline const float kHeadShotMotionTime = 1.0f;
	static inline const float kAscensionMotionTime = 3.0f;

	// 昇天の速さ
	static inline const float kAscensionSpeed = 0.03f;

	// 死亡演出で倒れ切る角度
	static inline const float kHeadShotMotionAngleEnd = -kPi / 2.0f;

	// 死亡演出中の回転速度
	static inline const float kDeathSpinSpeed = kPi * 6.0f;
};

float GraduallyChangingScaleA(float self, float target, float change);
float RoundTrip(float self, float target1, float target2, float& change, float second);