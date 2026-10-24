#pragma once
#include "KamataEngine.h"
#include "PlayerDirection.h"
#include <array>
#include "NumberRenderer.h"

// 前方宣言
class Player;

class GameHUD {
  public:
	void Initialize(Player* player, KamataEngine::Camera* camera);

	void Update();

	void Draw();

  private:
	//========================================================
	// HP表示
	//========================================================
	
	// ハートの画像
	std::vector<std::unique_ptr<KamataEngine::Sprite>> hpSprite_;
	std::vector<std::unique_ptr<KamataEngine::Sprite>> hpShadowSprite_;

	// ハート画像の大きさ
	static inline const float kHpSpriteWidth = 64.0f;
	static inline const float kHpSpriteHeight = 64.0f;

	// 1つ目のハートの位置
	static inline const float kHpSpritePosX = 10.0f;
	static inline const float kHpSpritePosY = 10.0f;

	// ハートとハートの距離
	static inline const float kHpSpriteDistance = kHpSpriteWidth + 5.0f;



	//============================================================
	// チャージゲージ
	//============================================================

	// チャージゲージ背景
	std::unique_ptr<KamataEngine::Sprite> chargeGaugeBackgroundSprite_;

	// チャージゲージ本体
	std::unique_ptr<KamataEngine::Sprite> chargeGaugeSprite_;

	// ゲージ背景サイズ
	static inline const float kGaugeBackgroundWidth = 28.0f;
	static inline const float kGaugeBackgroundHeight = 68.0f;

	// ゲージ本体サイズ
	static inline const float kGaugeWidth = 20.0f;
	static inline const float kGaugeHeight = 60.0f;

	// ゲージ背景座標（左下基準
	KamataEngine::Vector2 gaugeBackgroundPos_ = {};

	// ゲージ本体座標（左下基準
	KamataEngine::Vector2 gaugePos_ = {};

	// ゲージを表示する距離
	static inline const float kGaugeDistanceX = 80.0f;
	static inline const float kGaugeDistanceY = kGaugeWidth + 10;

	const KamataEngine::Vector4 chageStapOneColor{0.8f,0.3f,0.3f,1.0f};
	const KamataEngine::Vector4 chageStapTwoColor{1.0f, 0.1f, 0.1f, 1.0f};

	// チャージ完了時の色
	const KamataEngine::Vector4 chageCompleteColor{1.0f, 0.8f, 0.1f, 1.0f};

	//========================================================
	// 矢印周り
	//========================================================

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

	// 矢印を表示する距離
	static inline const float kArrowDistance = 120.0f;


	Player* player_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kScreenWide = 1280.0f;
	static inline const float kScreenHeight = 720.0f;

	//============================================================
	// 残弾表示
	//============================================================

	// 数字描画
	NumberRenderer numberRenderer_;

	// 残弾表示位置
	static inline const KamataEngine::Vector2 kAmmoPosition = {1100.0f, 30.0f};

	// 残弾文字サイズ
	static inline constexpr float kAmmoFontSize = 40.0f;

	// 残弾文字色
	static inline const KamataEngine::Vector4 kAmmoColor = {1.0f, 1.0f, 1.0f, 1.0f};
};
