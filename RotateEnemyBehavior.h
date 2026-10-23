#pragma once
#include "IEnemyBehavior.h"
#include "Vector3Util.h"
#include <KamataEngine.h>

// 前方宣言
class BaseEnemy;
class RotateEnemy;

// 振る舞い
enum class EnemyRotateBehavior {

	kWalk,           // 歩行
	kFlyRing,        // 上に回転
	kDeathDirection, // 死亡演出

	kUnknown
};



class EnemyRotateWalk : public IEnemiesBehavior {
public:
	explicit EnemyRotateWalk(RotateEnemy* rotateEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	RotateEnemy* rotateEnemy_;

	KamataEngine::Vector3 velocty_{};
	float walkTimer_ = 0.0f;
	float walkMotionTimer_ = 0.0f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.03f;

	// 回転までの周期[秒]
	static inline const float kWalkTime = 5.0f;
};

class EnemyRotateFlyRing : public IEnemiesBehavior {
public:
	explicit EnemyRotateFlyRing(RotateEnemy* rotateEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	RotateEnemy* rotateEnemy_;

	KamataEngine::Vector3 translation_{};
	float rotateTimer_ = 0.0f;
	float angul = -std::numbers::pi_v<float>;

	// 円の中心点
	KamataEngine::Vector3 circleCenterPoint = {0.0f, 0.0f, 0.0f};

	// 円の半径
	static inline const float kRadius = 2.5f;

	// 何秒で1回転するか
	static inline const float kRotateTime = 5.0f;
};

class EnemyRotateDeath : public IEnemiesBehavior {
public:
	explicit EnemyRotateDeath(RotateEnemy* rotateenemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	RotateEnemy* rotateEnemy_;

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