#include "GameHUD.h"
#include "Matrix4x4Util.h"
#include "Player.h"
#include <algorithm>
#include "GameScene.h"
#include <cmath>
#include "Ease.h"

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

void GameHUD::Initialize(Player* player, Camera* camera, GameScene *gameScene) {
	player_ = player;
	camera_ = camera;
	gameScene_ = gameScene;

	// 数字描画の初期化
	numberRenderer_.Initialize();

	uint32_t whiteTextureHandle = TextureManager::Load("white1x1.png");

	// 残弾アイコン
	uint32_t ammoTextureHandle = TextureManager::Load("Sprite/HUD/ammo.png");

	ammoIconSprite_.reset(Sprite::Create(ammoTextureHandle, kAmmoIconPosition));

	ammoIconSprite_->SetSize(kAmmoIconSize);

	// 操作のスプライト
	uint32_t spaceTextureHandle = TextureManager::Load("Sprite/HUD/space.png");
	uint32_t wsdTextureHandle = TextureManager::Load("Sprite/HUD/WSD.png");

	spaceSprite_.reset(Sprite::Create(spaceTextureHandle, kSpacePosition));
	wsdSprite_.reset(Sprite::Create(wsdTextureHandle, kWSDPosition));

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


	// 特殊効果説明スプライト
	uint32_t jumpTextureHandle = TextureManager::Load("Sprite/HUD/Jump.png");

	uint32_t downTextureHandle = TextureManager::Load("Sprite/HUD/Down.png");

	jumpSprite_.reset(Sprite::Create(jumpTextureHandle, {0.0f, 0.0f}));
	downSprite_.reset(Sprite::Create(downTextureHandle, {0.0f, 0.0f}));

	// 中央基準
	jumpSprite_->SetAnchorPoint({1.0f, 0.5f});
	downSprite_->SetAnchorPoint({1.0f, 0.5f});

	//============================================================
	// ウェーブゲージ
	//============================================================

	// Wave画像
	uint32_t waveTextureHandle = TextureManager::Load("Sprite/HUD/Wave.png");

	waveLabelSprite_.reset(Sprite::Create(waveTextureHandle, {0.0f, 0.0f}));

	waveLabelSprite_->SetAnchorPoint({1.0f, 0.0f});

	// ゲージの中心座標
	float waveGaugeCenterX = kWaveGaugePosition.x + kWaveGaugeWidth / 2.0f;

	// 背景の生成
	waveGaugeBackgroundSprite_.reset(Sprite::Create(whiteTextureHandle, kWaveGaugePosition));

	waveGaugeBackgroundSprite_->SetSize({kWaveGaugeWidth, kWaveGaugeHeight});

	waveGaugeBackgroundSprite_->SetColor({0.2f, 0.2f, 0.2f, 1.0f});

	// 左側のゲージを生成
	waveGaugeLeftSprite_.reset(Sprite::Create(whiteTextureHandle, {waveGaugeCenterX, kWaveGaugePosition.y}));

	// 右側のゲージを生成
	waveGaugeRightSprite_.reset(Sprite::Create(whiteTextureHandle, {waveGaugeCenterX, kWaveGaugePosition.y}));

	// 左側のゲージは右上を基準にする
	waveGaugeLeftSprite_->SetAnchorPoint({1.0f, 0.0f});

	// 右側のゲージは左上を基準にする
	waveGaugeRightSprite_->SetAnchorPoint({0.0f, 0.0f});

	// 左右の初期サイズ
	KamataEngine::Vector2 halfGaugeSize = {kWaveGaugeWidth / 2.0f, kWaveGaugeHeight};

	waveGaugeLeftSprite_->SetSize(halfGaugeSize);
	waveGaugeRightSprite_->SetSize(halfGaugeSize);

	// ゲージの色
	waveGaugeLeftSprite_->SetColor(kWaveGaugeColor);
	waveGaugeRightSprite_->SetColor(kWaveGaugeColor);

	// ウェーブ開始アニメーションの初期化
	previousWave_ = 0;
	waveAnimationTimer_ = 0.0f;
	waveNumberPosition_ = kWaveStartPosition;
	waveNumberFontSize_ = kWaveStartFontSize;



	
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

		chargeGaugeSprite_->SetColor(chageStapOneColor);

	} else if (chargeTime < Player::kChargeStepTwoTime) {

		// チャージ2段階目
		chargeRate = static_cast<float>(chargeTime - Player::kChargeStepOneTime) / static_cast<float>(Player::kChargeStepTwoTime - Player::kChargeStepOneTime);

		chargeGaugeSprite_->SetColor(chageStapTwoColor);

	} else {

		// チャージ完了
		chargeRate = 1.0f;

		chargeGaugeSprite_->SetColor(chageCompleteColor);
	}

	// 進行率を0.0f～1.0fに制限
	chargeRate = std::clamp(chargeRate, 0.0f, 1.0f);

	// ゲージの高さを更新
	chargeGaugeSprite_->SetSize({kGaugeWidth, kGaugeHeight * chargeRate});

	// ゲージの座標を更新
	chargeGaugeBackgroundSprite_->SetPosition(gaugeBackgroundPos_);
	chargeGaugeSprite_->SetPosition(gaugePos_);

	// ウェーブ開始アニメーションの更新
	UpdateWaveAnimation();

	//========================================================
	// 矢印周り
	//========================================================

	// プレイヤーの現在の向き
	Direction playerDirection = player_->GetDirection();

	// 1段階以上チャージされているか
	bool isSpecialChargeReady = chargeTime >= Player::kChargeStepOneTime;

	// 矢印を更新
	for (size_t i = 0; i < arrowsPos_.size(); ++i) {

		Direction arrowDirection = directions[i];

		int directionValue = static_cast<int>(arrowDirection);

		int directionX = directionValue / 10 - 5;
		int directionY = directionValue % 10 - 5;

		// 矢印の座標
		arrowsPos_[i].x = playerScreenPos.x + directionX * kArrowDistance;

		arrowsPos_[i].y = playerScreenPos.y - directionY * kArrowDistance;

		// 選択中の矢印か
		bool isSelected = (arrowDirection == playerDirection);

		// 特殊効果のある方向か
		bool isSpecialDirection = (arrowDirection == Direction::kUp || arrowDirection == Direction::kDown);

		// 特殊効果が有効か
		bool isSpecialActive = isSpecialChargeReady && isSpecialDirection;

		// 矢印の色
		Vector4 arrowColor = isSpecialActive ? kArrowSpecialColor : kArrowNormalColor;

		// 選択していない矢印は半透明
		if (!isSelected) {
			arrowColor.w = kArrowInactiveAlpha;
		}

		// 色を反映
		arrowSprite_[i]->SetColor(arrowColor);

		// サイズを反映
		if (isSelected) {
			arrowSprite_[i]->SetSize({kFocusArrowSize, kFocusArrowSize});
		} else {
			arrowSprite_[i]->SetSize({kBaseArrowSize, kBaseArrowSize});
		}

		// Spriteに座標を反映
		arrowSprite_[i]->SetPosition(arrowsPos_[i]);
	}

	// 特殊効果説明の位置更新
	// Jump! の表示位置
	jumpSprite_->SetPosition({playerScreenPos.x - kSpecialTextDistance, playerScreenPos.y + kArrowDistance});

	// Down! の表示位置
	downSprite_->SetPosition({playerScreenPos.x - kSpecialTextDistance, playerScreenPos.y - kArrowDistance});

	//============================================================
	// ウェーブゲージ
	//============================================================

	// ウェーブ残り時間を取得
	float remainingTime = gameScene_->GetWaveRemainingTime();

	// 残り時間の割合
	float remainingRate = remainingTime / GameScene::kWaveDuration;

	// 0.0f～1.0fに制限
	remainingRate = std::clamp(remainingRate, 0.0f, 1.0f);

	// 左右それぞれの幅を算出
	float halfWidth = kWaveGaugeWidth / 2.0f * remainingRate;

	// ゲージのサイズを更新
	waveGaugeLeftSprite_->SetSize({halfWidth, kWaveGaugeHeight});

	waveGaugeRightSprite_->SetSize({halfWidth, kWaveGaugeHeight});
}
void GameHUD::Draw() {
	Sprite::PreDraw();

	// 数字描画の使用位置をリセット
	numberRenderer_.BeginFrame();

	// 残弾表示
	// アイコン
	ammoIconSprite_->Draw();

	// プレイヤーの残弾数を取得
	int ammo = player_->GetAmmo();

	// 残弾数を描画
	numberRenderer_.DrawNumberRightAligned(ammo, kAmmoPosition, 2, kAmmoFontSize, kAmmoColor);

	// 操作スプライト
	wsdSprite_->Draw();
	spaceSprite_->Draw();

	// HP表示
	for (int i = 0; i < hpSprite_.size(); i++) {
		if (player_->GetHP() > i) {
			hpSprite_[i]->Draw();
		} else {
			hpShadowSprite_[i]->Draw();
		}
	}

	//============================================================
	// ウェーブ表示
	//============================================================

	// Wave画像のサイズを数字に合わせる
	waveLabelSprite_->SetSize({waveNumberFontSize_ * 3.0f, waveNumberFontSize_});

	// Wave画像を数字の左側に配置
	waveLabelSprite_->SetPosition({waveNumberPosition_.x - waveNumberFontSize_ / 2.0f - kWaveLabelGap, waveNumberPosition_.y});

	// Wave画像を描画
	waveLabelSprite_->Draw();

	// ウェーブ番号を取得
	int currentWave = gameScene_->GetCurrentWave();

	// ウェーブ番号を描画
	numberRenderer_.DrawNumberCenterAligned(currentWave, waveNumberPosition_, waveNumberFontSize_, kWaveNumberColor);

	// ゲージ背景を描画
	waveGaugeBackgroundSprite_->Draw();

	// 左右のゲージを描画
	waveGaugeLeftSprite_->Draw();
	waveGaugeRightSprite_->Draw();

	//============================================================
	// 敵数表示
	//============================================================

	// 現在の敵数を取得
	int remainingEnemyCount = gameScene_->GetRemainingEnemyCount();

	// ウェーブ開始時の総敵数を取得
	int totalEnemyCount = gameScene_->GetTotalEnemyCount();

	// 残り敵数とスラッシュを描画
	numberRenderer_.DrawNumberRightAligned(remainingEnemyCount, kEnemyCountPosition, 2, kEnemyCountFontSize, kEnemyCountColor, true);

	// 総敵数を描画
	numberRenderer_.DrawNumber(totalEnemyCount, kEnemyTotalCountPosition, kEnemyCountFontSize, kEnemyCountColor);



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

		// 1段階以上チャージしている場合
		if (player_->GetChargeTime() >= Player::kChargeStepOneTime) {

			Direction direction = player_->GetDirection();

			// Jump、Down
			if (direction == Direction::kUp) {
				downSprite_->Draw();
			} else if (direction == Direction::kDown) {
				jumpSprite_->Draw();
			}
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

void GameHUD::UpdateWaveAnimation() {

	// 現在のウェーブ番号を取得
	int currentWave = gameScene_->GetCurrentWave();

	// ウェーブ番号が変化した場合
	if (previousWave_ != currentWave) {

		// 現在のウェーブ番号を記録
		previousWave_ = currentWave;

		// アニメーションを最初から開始
		waveAnimationTimer_ = 0.0f;
	}

	// アニメーション経過時間を更新
	waveAnimationTimer_ += kWaveAnimationDeltaTime;

	//============================================================
	// 中央で静止
	//============================================================

	if (waveAnimationTimer_ < kWaveWaitDuration) {

		// 表示位置を中央に固定
		waveNumberPosition_ = kWaveStartPosition;

		// 表示サイズを大きくする
		waveNumberFontSize_ = kWaveStartFontSize;

		return;
	}

	//============================================================
	// 上部へ移動
	//============================================================

	// 移動開始後の経過時間
	float moveElapsedTime = waveAnimationTimer_ - kWaveWaitDuration;

	// 移動進行率
	float t = std::clamp(moveElapsedTime / kWaveMoveDuration, 0.0f, 1.0f);

	// イージング
	float easedT = Ease::EaseInOut(t);

	// X座標を補間
	waveNumberPosition_.x = std::lerp(kWaveStartPosition.x, kWaveNumberPosition.x, easedT);

	// Y座標を補間
	waveNumberPosition_.y = std::lerp(kWaveStartPosition.y, kWaveNumberPosition.y, easedT);

	// 文字サイズを補間
	waveNumberFontSize_ = std::lerp(kWaveStartFontSize, kWaveNumberFontSize, easedT);
}