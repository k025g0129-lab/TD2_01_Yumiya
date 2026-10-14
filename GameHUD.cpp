#include "GameHUD.h"
#include "Matrix4x4Util.h"
#include "Player.h"

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
}

void GameHUD::Update() {
	// プレイヤーのワールド座標
	Vector3 playerWorldPosition = player_->GetWorldPosition();

	// プレイヤーのスクリーン座標
	Vector2 playerScreenPosition = WorldToScreen(playerWorldPosition);

	// 矢印を表示する距離
	constexpr float kArrowDistance = 80.0f;

	// プレイヤーの現在の向き
	Direction playerDirection = player_->GetDirection();

	for (size_t i = 0; i < arrowsPos_.size(); ++i) {
		int directionValue = static_cast<int>(directions[i]);

		// Directionから方向ベクトルを復元
		int directionX = directionValue / 10 - 5;
		int directionY = directionValue % 10 - 5;

		// プレイヤーを中心に矢印を配置
		arrowsPos_[i].x = playerScreenPosition.x + directionX * kArrowDistance;
		arrowsPos_[i].y = playerScreenPosition.y - directionY * kArrowDistance;

		// 向いている方向の矢印だけ大きくする
		if (directions[i] == playerDirection) {
			arrowSprite_[i]->SetSize({kFocusArrowSize, kFocusArrowSize});
		} else {
			arrowSprite_[i]->SetSize({kBaseArrowSize, kBaseArrowSize});
		}

		// Spriteに座標を反映
		arrowSprite_[i]->SetPosition(arrowsPos_[i]);
	}
}
void GameHUD::Draw() {
	Sprite::PreDraw();

	for (std::unique_ptr<Sprite>& arrowSprite : arrowSprite_) {
		arrowSprite->Draw();
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