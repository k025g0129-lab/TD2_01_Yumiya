#include "BaseEnemy.h"
#include "Player.h"
#include "Matrix4x4Util.h"
#include <algorithm>

using namespace KamataEngine;

void BaseEnemy::Initialize(Camera* camera, Vector3 position) {
	InitializeCommon(camera, position);
	InitializeInternal();

	// 初期HPを最大HPとして保存
	maxHP_ = HP_;

	// HPゲージの初期化
	InitializeHPGauge();

	// 初回のワールド変換更新
	WorldTransformUpdate(worldTransform_);

	// HPゲージの初回更新
	UpdateHPGauge();

	objectColor_.Initialize();
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

	// HPゲージ更新
	UpdateHPGauge();
}

void BaseEnemy::Draw() {
	Model::PreDraw();

	model_->Draw(worldTransform_, *camera_, &objectColor_);

	Model::PostDraw();

	// HPゲージ描画
	DrawHPGauge();
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

void BaseEnemy::InitializeCommon(Camera* camera, Vector3 position) {
	#ifdef _DEBUG

	assert(camera);

	#endif // _DEBUG

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 初期配置座標
	worldTransform_.translation_ = position;

	// 引数で渡されたカメラをメンバ変数に保存
	camera_ = camera;

	isCollisionDisabled_ = false;
	isDead_ = false;
}

void BaseEnemy::InitializeHPGauge() {

	// 白画像を読み込む
	uint32_t whiteTextureHandle = TextureManager::Load("white1x1.png");

	// HPゲージ背景
	hpGaugeBackgroundSprite_.reset(Sprite::Create(whiteTextureHandle, {0.0f, 0.0f}));

	hpGaugeBackgroundSprite_->SetSize({kHPGaugeBackgroundWidth, kHPGaugeBackgroundHeight});

	// 背景色
	hpGaugeBackgroundSprite_->SetColor({0.15f, 0.15f, 0.15f, 0.85f});

	// HPゲージ本体
	hpGaugeSprite_.reset(Sprite::Create(whiteTextureHandle, {0.0f, 0.0f}));

	hpGaugeSprite_->SetSize({kHPGaugeWidth, kHPGaugeHeight});

	// HPゲージ色
	hpGaugeSprite_->SetColor({0.9f, 0.2f, 0.2f, 1.0f});

	// 左上を基準にして右方向へ伸びる
	hpGaugeBackgroundSprite_->SetAnchorPoint({0.0f, 0.0f});
	hpGaugeSprite_->SetAnchorPoint({0.0f, 0.0f});
}

void BaseEnemy::UpdateHPGauge() {

	// HPの割合を算出
	float hpRate = 0.0f;

	if (maxHP_ > 0) {
		hpRate = static_cast<float>(HP_) / static_cast<float>(maxHP_);
	}

	// 0.0f～1.0fに制限
	hpRate = std::clamp(hpRate, 0.0f, 1.0f);

	// HPに合わせてゲージの幅を変更
	hpGaugeSprite_->SetSize({kHPGaugeWidth * hpRate, kHPGaugeHeight});

	// 敵の頭上のワールド座標
	Vector3 gaugeWorldPosition = GetWorldPosition();
	gaugeWorldPosition.y += kHPGaugeOffsetY;

	// ビュー行列と射影行列を合成
	Matrix4x4 viewProjection = Multiply(camera_->matView, camera_->matProjection);

	// ワールド座標をNDC座標に変換
	Vector3 ndcPosition = Transform(gaugeWorldPosition, viewProjection);

	// スクリーン座標に変換
	Vector2 screenPosition = {};

	screenPosition.x = (ndcPosition.x + 1.0f) * 0.5f * kScreenWidth;

	screenPosition.y = (1.0f - ndcPosition.y) * 0.5f * kScreenHeight;

	// ゲージ背景の左上座標
	Vector2 gaugeBackgroundPos = {screenPosition.x - kHPGaugeBackgroundWidth / 2.0f, screenPosition.y - kHPGaugeBackgroundHeight / 2.0f};

	// ゲージ本体の左上座標
	Vector2 gaugePos = {gaugeBackgroundPos.x + (kHPGaugeBackgroundWidth - kHPGaugeWidth) / 2.0f, gaugeBackgroundPos.y + (kHPGaugeBackgroundHeight - kHPGaugeHeight) / 2.0f};

	// スプライトに座標を反映
	hpGaugeBackgroundSprite_->SetPosition(gaugeBackgroundPos);
	hpGaugeSprite_->SetPosition(gaugePos);
}

void BaseEnemy::DrawHPGauge() {

	// 死亡した敵やHPが0の敵は描画しない
	if (isDead_ || HP_ <= 0) {
		return;
	}

	// スプライト描画開始
	Sprite::PreDraw();

	// ゲージ背景
	hpGaugeBackgroundSprite_->Draw();

	// ゲージ本体
	hpGaugeSprite_->Draw();

	// スプライト描画終了
	Sprite::PostDraw();
}