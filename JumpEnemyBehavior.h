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
	bool isGround_ = true;
	bool preIsGround_ = true;

	static inline const int kJumpCountMax = 7;
	static inline const float kJumpPower = 0.25f;
	static inline const float kGravity = 0.02f;

	// 歩行モーションの最初の角度
	static inline const float kWalkMotionAngleStart = kPi / 4.0f;

	// 歩行モーション終わりの角度
	static inline const float kWalkMotionAngleEnd = -kWalkMotionAngleStart;

	// 歩行アニメーションの周期[秒]
	static inline const float kWalkMotionTime = 2.0f;

	// 回転までの周期[秒]
	static inline const float kWalkTime = 5.0f;
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
	static inline const float kHighJumpPower = 0.5f;
	static inline const float kGravity = 0.02f;

	KamataEngine::Vector3 velocity_{};
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
