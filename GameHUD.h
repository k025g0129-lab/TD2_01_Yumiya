#pragma once
#include "KamataEngine.h"
#include "PlayerDirection.h"
#include <array>

// 前方宣言
class Player;

class GameHUD {
  public:
	void Initialize(Player* player, KamataEngine::Camera* camera);

	void Update();

	void Draw();

  private:
	// worldPositionをスクリーン座標に変換する
	KamataEngine::Vector2 WorldToScreen(const KamataEngine::Vector3& worldPosition) const;

	// 矢印の座標
	std::array<KamataEngine::Vector2, kCountOfDirection> arrowsPos_ = {};

	// 矢印の画像
	std::array<std::unique_ptr<KamataEngine::Sprite>, kCountOfDirection> arrowSprite_;

	// 通常時の矢印のサイズ
	static inline const float kBaseArrowSize = 24.0f;

	// 向いている方向の矢印のサイズ
	static inline const float kFocusArrowSize = 48.0f;


	Player* player_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kScreenWide = 1280.0f;
	static inline const float kScreenHeight = 720.0f;
};
