#pragma once
#include "Collision.h"
#include "KamataEngine.h"
#include "LRDirection.h"
#include "MapChipField.h"
#include "PlayerBullet.h"
#include "PlayerDirection.h"

#include <vector>

// 前方宣言
class MapChipField;
class BaseEnemy;

enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

struct CollisionMapInfo {
	bool isHitCeiling = false;
	bool onGround = false;
	bool isHitWall = false;
	KamataEngine::Vector3 moveDistance = {};
};

struct JumpThroughPoint {
	float yPos_ ;
	bool isOver;
	bool isPreOver;
};

class Player {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	// 初期化処理
	void Initialize(KamataEngine::Camera* camera, KamataEngine::Vector3 position);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// 敵と当たった時の処理
	void OnCollision();

	// ノックバックを要求
	void RequestKnockback() { isKnockbackRequested_ = true; }

	// 調整項目を登録
	static void RegisterGlobalVariables();

	// 調整項目を適用
	static void ApplyGlobalVariables();

	//============================================================
	// ゲッター・セッター
	//============================================================

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// 画面端挟まれ死亡判定を有効にするか
	void SetScreenSqueezeDeathEnabled(bool enabled) { isScreenSqueezeDeathEnabled_ = enabled; }

	const KamataEngine::WorldTransform& GetWorldTransform();
	KamataEngine::Vector3 GetWorldPosition() const;
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	// 敵とのあたり判定
	AABB GetAABB() const;

	// 死亡しているか
	bool IsDead() const { return isDead_; }

	LRDirection GetLRDirection() const { return lrDirection_; }

	Direction GetDirection() const { return static_cast<Direction>(direction_); }

	// 弾の一覧を取得
	const std::vector<PlayerBullet*>& GetBullets() const { return bullets_; }

	int GetChargeTime() const { return chargeTime_; }

	// パブリックメンバ変数
	// チャージ1段階目に必要な時間
	static inline const int kChargeStepOneTime = 30;

	// チャージ2段階目に必要な時間
	static inline const int kChargeStepTwoTime = kChargeStepOneTime + 30;

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;

	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 接地フラグ
	bool isGround_ = true;

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	// 重力加速度
	static inline const float kGravityAcceleration = 0.07f;

	// 最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;

	// 接地確認用の微小な下方向オフセット
	static inline const float kGroundCheckOffset = 0.1f;

	//============================================================
	// HP・被ダメージ周り
	//============================================================

	// 最大HP
	static inline const int kMaxHP = 3;

	// HP
	int HP_ = kMaxHP;

	// 被弾後無敵の残り時間
	float damageInvincibleTimer_ = 0.0f;

	// 被弾後の無敵時間
	static inline float kDamageInvincibleTime = 0.75f;

	// 被弾後無敵を更新する
	void UpdateDamageInvincibility();

	// モデル色
	KamataEngine::ObjectColor objectColor_;

	// 現在のモデル色
	KamataEngine::Vector4 objectColorValue_ = {1.0f, 1.0f, 1.0f, 1.0f};

	// ヒットフラッシュの残り時間
	float damageColorTimer_ = 0.0f;

	// ヒットフラッシュの時間
	static inline const float kDamageColorTime = 0.15f;

	// 無敵時間中の点滅周期
	static inline const float kDamageBlinkInterval = 0.10f;

	// 被ダメージ表示更新
	void UpdateDamageVisual();

	//============================================================
	// 振る舞い
	//============================================================

	enum class Behavior {
		kRoot,         // 通常状態
		kCharge,       // 溜める
		kKnockback,    // ノックバック
		kSqueezeDeath, // 挟まれ死亡演出
		kUnknown
	};

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	//============================================================
	// 死亡周り
	//============================================================

	// 死亡の種類
	enum class DeathType {
		kNone,
		kNormal,  // 通常死
		kSqueeze, // 挟まれ
	};

	DeathType deathType_ = DeathType::kNone;

	// デスフラグ
	bool isDead_ = false;

	// 死亡演出が終わったか
	bool isDeathMotionFinished_ = false;

	// 描画するべきか
	bool ShouldDraw() const;

	//============================================================
	// 通常状態
	//============================================================

	// 通常行動の初期化
	void BehaviorRootInitialize();

	// 通常行動更新
	void BehaviorRootUpdate();

	// 通過ポイントの初期化
	void JumpThroughInitialize();

	// 通過ポイント更新
	void JumpThroughUpdate();

	//超えているかの共通処理
	void IsJumpThroughPointOver(JumpThroughPoint *jumpThroughPoint);

	// 歩行の最高速
	static inline float kLimitRunSpeed = 0.3f;

	// 接地時に減衰するx速度
	static inline float kAttenuationLanding = 0.05f;

	// 壁接触時に減衰するx速度
	static inline float kAttenuationWall = 0.2f;

	// 旋回にかかる時間<秒>
	static inline float kTurnTime = 0.3f;

	// ジャンプでの通り抜け
	bool isJumpThrough_ = false;

	//通過ポイントより上か
	bool isOverJumpThroughPoint = false;
	bool isPreOverJumpThroughPoint = false;

	//ジャンプポイント数(後々変更するか消す可能性あり)
	static inline const int maxJumpThroughPointIndex = 10;

	JumpThroughPoint jumpThroughPoint_[maxJumpThroughPointIndex]{};
	JumpThroughPoint jumpUpperLimitsPoint{};
	JumpThroughPoint jumpLowerLimitsPoint{};


	// 弓のチャージ時間
	int chargeTime_ = 0;

	// チャージ0段階目でのジャンプ力
	static inline const float kStepZeroJumpPower = 0.6f;

	// チャージ1段階目でのジャンプ力
	static inline const float kStepOneJumpPower = 1.0f;

	// チャージ2段階目でのジャンプ力
	static inline const float kStepTwoJumpPower = 1.2f;

	//============================================================
	// 溜め状態
	//============================================================

	// 溜める行動の初期化
	void BehaviorChargeInitialize();

	// 溜める行動更新
	void BehaviorChargeUpdate();

	// チャージ段階
	int GetChargeStep() const;

	//============================================================
	// ノックバック
	//============================================================

	// ノックバックのフェーズ
	enum class KnockbackPhase {
		kKnockback, // 吹っ飛ばされる
		kAfter      // 後隙
	};

	// ノックバックの初期化
	void BehaviorKnockInitialize();

	// ノックバックの更新
	void BehaviorKnockUpdate();

	// ノックバックの吹っ飛ばされている時間かどうか
	bool IsKnockbackInvincible() const { return (behavior_ == Behavior::kKnockback && knockbackPhase_ == KnockbackPhase::kKnockback); }

	// ノックバック要求フラグ
	bool isKnockbackRequested_ = false;

	// ノックバックのフェーズ
	KnockbackPhase knockbackPhase_ = KnockbackPhase::kKnockback;

	// ノックバック経過時間
	float knockbackParameter_ = 0.0f;

	// ノックバックで吹っ飛ばされる時間
	static inline float kKnockbackTime = 0.15f;

	// ノックバック後の硬直時間
	static inline float kKnockbackAfterTime = 0.1f;

	// ノックバックの横速度
	static inline float kKnockbackSpeed = 0.3f;

	// ノックバック後の減速率
	static inline float kKnockbackAttenuation = 0.15f;

	//============================================================
	// マップチップ回り
	//============================================================

	// マップチップとの衝突を解決する
	void ResolveMapChipCollision(const KamataEngine::Vector3& velocity);

	void MapChipCollision(CollisionMapInfo& info);
	void MapChipCollisionTop(CollisionMapInfo& info);
	void MapChipCollisionBottom(CollisionMapInfo& info);
	void MapChipCollisionLeft(CollisionMapInfo& info);
	void MapChipCollisionRight(CollisionMapInfo& info);

	// 衝突結果を反映して移動させる
	void ApplyCollisionResult(const CollisionMapInfo& info);

	// 天井に接触している時の処理
	void HandleCeilingCollision(const CollisionMapInfo& info);

	// 地面に接触している時の処理
	void HandleGroundCollision(const CollisionMapInfo& info);

	// 壁に接触してるときの処理
	void HandleWallCollision(const CollisionMapInfo& info);

	// 地面に触れてるときの処理
	void ChangeGroundState(const CollisionMapInfo& info);

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// めり込み防止用の微小な余白
	static inline const float kBlank = 0.05f;

	//============================================================
	// 強制スクロール
	//============================================================

	// 画面端からはみ出さないように補正する
	void CorrectPositionInScreen();

	// 画面端と壁に挟まれたか確認する
	void CheckScreenSqueezeDeath();

	// 画面左端に接触しているか
	bool IsTouchingLeftScreenEdge();

	// 右側にブロックがあるか
	bool IsBlockOnRightSide();

	// 挟まれ死亡を開始する
	void StartSqueezeDeath();

	// 挟まれ死亡演出の初期化
	void BehaviorSqueezeDeathInitialize();

	// 挟まれ死亡演出の更新
	void BehaviorSqueezeDeathUpdate();

	// 画面端挟まれ死亡判定を有効にするか
	bool isScreenSqueezeDeathEnabled_ = false;

	// 挟まれ死亡演出タイマー
	float squeezeDeathTimer_ = 0.0f;

	// 画面端接触判定の許容誤差
	static inline const float kScreenEdgeCheckTolerance = 0.05f;

	// 壁確認用の微小なオフセット
	static inline const float kScreenSqueezeCheckOffset = 0.05f;

	// 挟まれ死亡時の上方向初速
	static inline const float kSqueezeDeathJumpVelocity = 0.5f;

	// 挟まれ死亡時の重力
	static inline const float kSqueezeDeathGravity = 0.015f;

	// 挟まれ死亡時のX回転速度
	static inline const float kSqueezeDeathRotationSpeed = kPi * 2.0f;

	// 挟まれ死亡演出時間
	static inline const float kSqueezeDeathMotionTime = 1.5f;

	// 画面端補正用の範囲
	static inline const float kScreenLeftLimit = -11.0f;
	static inline const float kScreenRightLimit = 11.0f;
	static inline const float kScreenBottomLimit = -8.0f;
	static inline const float kScreenTopLimit = 10.0f;

	/// =============================
	/// 方向制御
	/// =============================

	// 向いている方向
	int direction_ = Direction::kRight;

	/// <summary>
	/// 入力による方向の更新
	/// </summary>
	void PlayerDirectionUpdate();

	/// =============================
	/// 弾
	/// =============================
	
	// 配列
	std::vector<PlayerBullet*> bullets_;
};