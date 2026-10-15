#include "BaseEnemy.h"
#include "Player.h"

using namespace KamataEngine;

void BaseEnemy::Initialize(Model* model, Camera* camera, Vector3 position) {
	InitializeCommon(model, camera, position);
	InitializeInternal();
}

void BaseEnemy::Update() {
	// 振る舞いの変更処理
	ChangeBehavior();

	// 振る舞いに応じた更新
	behavior_->Update();

	// 振る舞いに応じたモーションの更新
	behavior_->UpdateMotion();

	// ワールド変換の更新
	WorldTransformUpdate(worldTransform_);
}

void BaseEnemy::Draw() {
	Model::PreDraw();

	model_->Draw(worldTransform_, *camera_);

	Model::PostDraw();
}

void BaseEnemy::Move(const Vector3& velocity) { worldTransform_.translation_ += velocity; }

AABB BaseEnemy::GetAABB() const {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb{};
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

Vector3 BaseEnemy::GetWorldPosition() const {
	Vector3 worldPosition{};

	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];

	return worldPosition;
}

void BaseEnemy::InitializeCommon(Model* model, Camera* camera, Vector3 position) {
	#ifdef _DEBUG

	assert(model);
	assert(camera);

	#endif // _DEBUG

	// 引数で渡されたモデルとテクスチャハンドルをメンバ変数に保存
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 初期配置座標
	worldTransform_.translation_ = position;

	// 引数で渡されたカメラをメンバ変数に保存
	camera_ = camera;

	isCollisionDisabled_ = false;
	isDead_ = false;
}