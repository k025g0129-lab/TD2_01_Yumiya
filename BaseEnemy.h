#pragma once
#include<KamataEngine.h>
#include"Collision.h"
#include "IEnemyBehavior.h"
#include <memory>

// 前方宣言
class Player;
class GameScene;
class PlayerBullet;
enum class EnemyBehavior;

class BaseEnemy {
  public:
	// 仮想デストラクタ
	virtual ~BaseEnemy() = default;

	void Initialize(KamataEngine::Camera* camera, KamataEngine::Vector3 position);
	void Update();
	virtual void Draw();
	virtual void OnCollision() = 0;
	virtual void OnCollisionBullet(int bulletAttack) = 0;

	AABB GetAABB() const;
	KamataEngine::Vector3 GetWorldPosition() const;
	// あたり判定無効かどうか
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }
	// 死亡しているか
	bool IsDead() const { return isDead_; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	//============================================================
	// ステートからの操作
	//============================================================

	// 移動
	void Move(const KamataEngine::Vector3& velocity);

	// 座標設定
	void SetTranslation(KamataEngine::Vector3 translation) { worldTransform_.translation_ = translation; }

	// 座標取得
	const KamataEngine::Vector3& GetTranslation() const { return worldTransform_.translation_; }

	// X軸回転設定
	void SetRotationX(float angle) { worldTransform_.rotation_.x = angle; }

	// X軸回転取得
	float GetRotationX() const { return worldTransform_.rotation_.x; }

	// Y軸回転設定
	void SetRotationY(float angle) { worldTransform_.rotation_.y = angle; }

	// Y軸回転取得
	float GetRotationY() const { return worldTransform_.rotation_.y; }

	// Z軸回転設定
	void SetRotationZ(float angle) { worldTransform_.rotation_.z = angle; }

	// Z軸回転取得
	float GetRotationZ() const { return worldTransform_.rotation_.z; }


	// X軸サイズ設定
	void SetScaleX(float size) { worldTransform_.scale_.x = size; }

	// X軸サイズ取得
	float GetScaleX() const { return worldTransform_.scale_.x; }

	// Y軸サイズ設定
	void SetScaleY(float size) { worldTransform_.scale_.y = size; }

	// Y軸サイズ取得
	float GetScaleY() const { return worldTransform_.scale_.y; }

	// Z軸サイズ設定
	void SetScaleZ(float size) { worldTransform_.scale_.z = size; }

	// Z軸サイズ取得
	float GetScaleZ() const { return worldTransform_.scale_.z; }

	// 速度設定
	void SetVelocity(const KamataEngine::Vector3& velocity) { velocity_ = velocity; }

	// 速度取得
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	// 当たり判定無効フラグ設定
	void SetCollisionDisabled(bool isDisabled) { isCollisionDisabled_ = isDisabled; }

	// 死亡フラグ設定
	void SetDead(bool isDead) { isDead_ = isDead; }

	KamataEngine::Vector4 const GetObjectColorRGBA() { return objectColor_.GetColor(); };
	void SetObjectColorRGBA(KamataEngine::Vector4 objectColorRGBA) { objectColor_.SetColor(objectColorRGBA); };

  protected:
	// 初期化の共通処理
	virtual void InitializeInternal() = 0;

	// 振る舞いの変更処理
	virtual void ChangeBehavior() = 0;

	// 振る舞い
	std::unique_ptr<IEnemiesBehavior> behavior_;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	//カラー
	KamataEngine::ObjectColor objectColor_;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// キャラクターの当たり判定サイズ
	static inline float kWidth = 0.8f;
	static inline float kHeight = 0.8f;

	// あたり判定無効(死亡演出中)
	bool isCollisionDisabled_ = false;

	// デスフラグ
	bool isDead_ = false;

	// 敵のHP。敵に応じて初期化でそれぞれ変える
	int HP_ = 3;

	// 敵の最大HP
	int maxHP_ = 3;

	//============================================================
	// HPゲージ
	//============================================================

	// HPゲージ背景
	std::unique_ptr<KamataEngine::Sprite> hpGaugeBackgroundSprite_;

	// HPゲージ本体
	std::unique_ptr<KamataEngine::Sprite> hpGaugeSprite_;

	// ゲージ背景のサイズ
	static inline const float kHPGaugeBackgroundWidth = 64.0f;
	static inline const float kHPGaugeBackgroundHeight = 12.0f;

	// ゲージ本体のサイズ
	static inline const float kHPGaugeWidth = 60.0f;
	static inline const float kHPGaugeHeight = 8.0f;

	// 敵の中心からゲージまでの高さ（ワールド座標）
	static inline const float kHPGaugeOffsetY = 0.9f;

	// 画面サイズ
	static inline const float kScreenWidth = 1280.0f;
	static inline const float kScreenHeight = 720.0f;

	// ゲージ初期化
	void InitializeHPGauge();

	// ゲージ更新
	void UpdateHPGauge();

	// ゲージ描画
	void DrawHPGauge();

  private:
	void InitializeCommon(KamataEngine::Camera* camera, KamataEngine::Vector3 position);
};

