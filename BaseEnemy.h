#pragma once
#include<KamataEngine.h>
#include"Collision.h"
#include "IEnemyBehavior.h"

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
	virtual void OnCollision(Player* player) = 0;
	virtual void OnCollisionBullet() = 0;

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

  private:
	void InitializeCommon(KamataEngine::Camera* camera, KamataEngine::Vector3 position);
};

