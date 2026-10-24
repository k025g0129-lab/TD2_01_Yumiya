#pragma once
#include "IEnemyBehavior.h"
#include "Vector3Util.h"
#include <KamataEngine.h>
#include "MapChipField.h"

// 前方宣言
class BaseEnemy;
class JumpEnemy;

// 振る舞い
enum class EnemyJumpBehavior {

	kJumpWalk,           // ジャンプ歩行
	kHighJump,       // 上の段に移動
	kDeathDirection, // 死亡演出

	kUnknown
};

class EnemyJumpWalk : public IEnemiesBehavior {
public:
	explicit EnemyJumpWalk(JumpEnemy* jumpEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	JumpEnemy* jumpEnemy_;
	MapChipField* mapChipField_;

	KamataEngine::Vector3 velocty_{};
	float walkTimer_ = 0.0f;
	float walkMotionTimer_ = 0.0f;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.015f;

	//ジャンプ
	int jumpCount_ = 0;
	bool isJump_ = true;
	bool preIsGround_ = true;

	static inline const int kJumpCountMax = 7;
	static inline const float kJumpPower = 0.125f;
	static inline const float kGravity = 0.005f;


};


class EnemyHighJump : public IEnemiesBehavior {
public:
	explicit EnemyHighJump(JumpEnemy* jumpEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:

	enum class HighJumpPhase {

		kCharge,
		kJump,
	};

	JumpEnemy* jumpEnemy_;
	MapChipField* mapChipField_;

	KamataEngine::Vector3 translation_{};
	HighJumpPhase highJumpPhase = HighJumpPhase::kCharge;

	float chargeTimer_ = 0.0f;

	static inline const float kChargeTime = 100.7f;
	static inline const float kHighJumpPower = 0.35f;
	static inline const float kGravity = 0.01f;

	KamataEngine::Vector3 velocity_{};

	//モーション

};

class EnemyJumpDeath : public IEnemiesBehavior {
public:
	explicit EnemyJumpDeath(JumpEnemy* jumpEnemy);
	void Initialize() override;
	void Update() override;
	void UpdateMotion() override;

private:
	JumpEnemy* jumpEnemy_;
	MapChipField* mapChipField_;
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

float GraduallyChangingScale(float self, float target, float change);
void ShakeScale(float& self, float shake1, float shake2);