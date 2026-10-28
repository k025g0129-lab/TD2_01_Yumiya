#pragma once
#include "IEnemyBehavior.h"
#include<KamataEngine.h>
#include "Vector3Util.h"

// 前方宣言
class BaseEnemy;
class Enemy;

// 振る舞い
enum class EnemyBehavior {

	kWalk,           // 歩行
	kDeathDirection, // 死亡演出

	kUnknown
};

class EnemyWalk : public IEnemiesBehavior{
  public:
	explicit EnemyWalk(BaseEnemy* enemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

  private:
	BaseEnemy* enemy_;

	KamataEngine::Vector3 velocty_{};
	float walkTimer_ = 0.0f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.01f;

	// 歩行モーションの最初の角度
	static inline const float kWalkMotionAngleStart = kPi / 4.0f;

	// 歩行モーション終わりの角度
	static inline const float kWalkMotionAngleEnd = -kWalkMotionAngleStart;

	// 歩行アニメーションの周期[秒]
	static inline const float kWalkMotionTime = 2.0f;
};


class EnemyDeath : public IEnemiesBehavior {
public:
	explicit EnemyDeath(BaseEnemy* enemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	BaseEnemy* enemy_;
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
