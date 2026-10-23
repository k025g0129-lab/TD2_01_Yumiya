#include "GameHUD.h"
#include "Matrix4x4Util.h"
#include "Player.h"
#include <algorithm>

using namespace KamataEngine;

namespace {

// 各矢印に対応する方向
const std::array<Direction, kCountOfDirection> directions = {
	Direction::kUp,
	Direction::kRightUp,
	Direction::kRight,
	Direction::kRightDown,
	Direction::kDown,
};
}

void GameHUD::Initialize(Player* player, Camera* camera) {
	player_ = player;
	camera_ = camera;

	//========================================================
	// HP
	//========================================================

	uint32_t hpTextureHandle = TextureManager::Load("Sprite/HUD/life.png");
	uint32_t hpShadowTextureHandle = TextureManager::Load("Sprite/HUD/life_Shadow.png");

	// HPスプライト数をプレイヤーの最大HPに合わせる
	hpSprite_.resize(player_->kMaxHP);
	hpShadowSprite_.resize(player_->kMaxHP);

	for (int i = 0; i < hpSprite_.size(); i++) {
		Vector2 hpPos = {kHpSpritePosX + (kHpSpriteDistance * i), kHpSpritePosY};

		hpSprite_[i].reset(Sprite::Create(hpTextureHandle, hpPos));
		hpSprite_[i]->SetSize({kHpSpriteWidth, kHpSpriteHeight});

		hpShadowSprite_[i].reset(Sprite::Create(hpShadowTextureHandle, hpPos));
		hpShadowSprite_[i]->SetSize({kHpSpriteWidth, kHpSpriteHeight});
	}



	//============================================================
	// チャージゲージ
	//============================================================

	uint32_t whiteTextureHandle = TextureManager::Load("white1x1.png");

	chargeGaugeBackgroundSprite_.reset(Sprite::Create(whiteTextureHandle, gaugeBackgroundPos_));

	chargeGaugeBackgroundSprite_->SetSize({kGaugeBackgroundWidth, kGaugeBackgroundHeight});

	chargeGaugeSprite_.reset(Sprite::Create(whiteTextureHandle, gaugePos_));

	chargeGaugeSprite_->SetSize({0.0f, 0.0f});

	chargeGaugeSprite_->SetColor(chageStapOneColor);

	// 画像左下を位置・回転の基準にする
	chargeGaugeBackgroundSprite_->SetAnchorPoint({0.0f, 1.0f});
	chargeGaugeSprite_->SetAnchorPoint({0.0f, 1.0f});

	//========================================================
	// 矢印
	//========================================================

	uint32_t arrowTextureHandle = TextureManager::Load("Sprite/HUD/arrow.png");

	for (int i = 0; i < arrowSprite_.size(); i++) {
		arrowSprite_[i].reset(Sprite::Create(arrowTextureHandle, arrowsPos_[i]));
		arrowSprite_[i]->SetSize({kBaseArrowSize, kBaseArrowSize});
		// 画像中央を位置・回転の基準にする
		arrowSprite_[i]->SetAnchorPoint({0.5f, 0.5f});

		int directionValue = static_cast<int>(directions[i]);

		// Directionから方向ベクトルを復元
		int directionX = directionValue / 10 - 5;
		int directionY = directionValue % 10 - 5;

		// 方向ベクトルから矢印の回転角を算出
		float rotation = std::atan2(static_cast<float>(-directionY), static_cast<float>(directionX));

		arrowSprite_[i]->SetRotation(rotation);
	}

	// 更新することで最初のフレームから位置を補正
	Update();
}

void GameHUD::Update() {
	// プレイヤーのスクリーン座標
	Vector2 playerScreenPos = WorldToScreen(player_->GetWorldPosition());

	//========================================================
	// チャージゲージ
	//========================================================

	// プレイヤーのチャージ時間を取得
	int chargeTime = player_->GetChargeTime();

	// ゲージ背景の位置
	gaugeBackgroundPos_ = {playerScreenPos.x - kGaugeDistanceX, playerScreenPos.y + kGaugeDistanceY};

	// ゲージ本体の位置
	// 背景よりも上下左右に4ピクセル内側に配置する
	gaugePos_ = {gaugeBackgroundPos_.x + (kGaugeBackgroundWidth - kGaugeWidth) / 2.0f, gaugeBackgroundPos_.y - (kGaugeBackgroundHeight - kGaugeHeight) / 2.0f};

	// チャージ進行率
	float chargeRate = 0.0f;

	// チャージ1段階目
	if (chargeTime < Player::kChargeStepOneTime) {

		chargeRate = static_cast<float>(chargeTime) / static_cast<float>(Player::kChargeStepOneTime);

		// 1段階目の色
		chargeGaugeSprite_->SetColor(chageStapOneColor);

	} else {

		// チャージ2段階目
		chargeRate = static_cast<float>(chargeTime - Player::kChargeStepOneTime) / static_cast<float>(Player::kChargeStepTwoTime - Player::kChargeStepOneTime);

		// 2段階目の色
		chargeGaugeSprite_->SetColor(chageStapTwoColor);
	}

	// 進行率を0.0f～1.0fに制限
	chargeRate = std::clamp(chargeRate, 0.0f, 1.0f);

	// ゲージの高さを更新
	chargeGaugeSprite_->SetSize({kGaugeWidth, kGaugeHeight * chargeRate});

	// ゲージの座標を更新
	chargeGaugeBackgroundSprite_->SetPosition(gaugeBackgroundPos_);
	chargeGaugeSprite_->SetPosition(gaugePos_);


	//========================================================
	// 矢印周り
	//========================================================

	// プレイヤーの現在の向き
	Direction playerDirection = player_->GetDirection();

	for (size_t i = 0; i < arrowsPos_.size(); ++i) {
		int directionValue = static_cast<int>(directions[i]);

		// Directionから方向ベクトルを復元
		int directionX = directionValue / 10 - 5;
		int directionY = directionValue % 10 - 5;

		// プレイヤーを中心に矢印を配置
		arrowsPos_[i].x = playerScreenPos.x + directionX * kArrowDistance;
		arrowsPos_[i].y = playerScreenPos.y - directionY * kArrowDistance;

		// 向いている方向の矢印だけ大きくする
		if (directions[i] == playerDirection) {
			arrowSprite_[i]->SetSize({kFocusArrowSize, kFocusArrowSize});

			// 向いている矢印は普通の色にする
			arrowSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		} else {
			arrowSprite_[i]->SetSize({kBaseArrowSize, kBaseArrowSize});

			// 向いてない矢印は薄くする
			arrowSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, 0.75f});
		}

		// Spriteに座標を反映
		arrowSprite_[i]->SetPosition(arrowsPos_[i]);
	}
}
void GameHUD::Draw() {
	Sprite::PreDraw();

	// HP表示
	for (int i = 0; i < hpSprite_.size(); i++) {
		if (player_->GetHP() > i) {
			hpSprite_[i]->Draw();
		} else {
			hpShadowSprite_[i]->Draw();
		}
	}

	// 生きてる間だけ表示
	if (!player_->IsDead()) {

		// チャージゲージの背景
		chargeGaugeBackgroundSprite_->Draw();

		// チャージゲージ本体
		chargeGaugeSprite_->Draw();

		// 矢印
		for (std::unique_ptr<Sprite>& arrowSprite : arrowSprite_) {
			arrowSprite->Draw();
		}
	}

	Sprite::PostDraw();
}

Vector2 GameHUD::WorldToScreen(const Vector3& worldPosition) const {

	// ビュー行列と射影行列を合成
	Matrix4x4 viewProjection = Multiply(camera_->matView, camera_->matProjection);

	// ワールド座標をNDC座標へ変換
	Vector3 ndcPosition = Transform(worldPosition, viewProjection);

	// NDC座標をスクリーン座標へ変換
	Vector2 screenPosition = {};

	screenPosition.x = (ndcPosition.x + 1.0f) * 0.5f * kScreenWide;
	screenPosition.y = (1.0f - ndcPosition.y) * 0.5f * kScreenHeight;

	return screenPosition;
}