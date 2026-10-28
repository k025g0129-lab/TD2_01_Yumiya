#pragma once
#include "KamataEngine.h"
#include "PlayerDirection.h"
#include <array>
#include "NumberRenderer.h"

// 前方宣言
class Player;
class GameScene;

class GameHUD {
  public:
	void Initialize(Player* player, KamataEngine::Camera* camera, GameScene *gameScene);

	void Update();

	void Draw();


  private:
	// ゲームシーン
	GameScene* gameScene_ = nullptr;

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
	static inline const float kGaugeDistanceX = 120.0f;
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

	// 矢印の通常色
	static inline const KamataEngine::Vector4 kArrowNormalColor = {1.0f, 0.85f, 0.0f, 1.0f};

	// 特殊効果が使用可能な矢印の色
	static inline const KamataEngine::Vector4 kArrowSpecialColor = {1.0f, 0.0f, 0.0f, 1.0f};

	// 非選択中の透明度
	static inline constexpr float kArrowInactiveAlpha = 0.75f;

	// 上方向の特殊効果説明
	std::unique_ptr<KamataEngine::Sprite> jumpSprite_;

	// 下方向の特殊効果説明
	std::unique_ptr<KamataEngine::Sprite> downSprite_;

	// 矢印から文字までの距離
	static inline constexpr float kSpecialTextDistance = 30.0f;


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
	static inline const KamataEngine::Vector2 kAmmoPosition = {1180.0f, 30.0f};

	// 残弾文字サイズ
	static inline constexpr float kAmmoFontSize = 40.0f;

	// 残弾文字色
	static inline const KamataEngine::Vector4 kAmmoColor = {1.0f, 1.0f, 1.0f, 1.0f};

	// 残弾アイコン
	std::unique_ptr<KamataEngine::Sprite> ammoIconSprite_;

	// 残弾アイコンの位置とサイズ
	static inline const KamataEngine::Vector2 kAmmoIconPosition = {1040.0f, 20.0f};
	static inline const KamataEngine::Vector2 kAmmoIconSize = {128.0f, 64.0f};


	//============================================================
	// ウェーブ表示
	//============================================================

	// Wave文字画像
	std::unique_ptr<KamataEngine::Sprite> waveLabelSprite_;

	// Wave画像と数字の間隔
	static inline constexpr float kWaveLabelGap = 8.0f;

	// ウェーブ番号の表示位置
	static inline const KamataEngine::Vector2 kWaveNumberPosition = {800.0f, 10.0f};

	// ウェーブ番号の文字サイズ
	static inline constexpr float kWaveNumberFontSize = 36.0f;

	// ウェーブ番号の文字色
	static inline const KamataEngine::Vector4 kWaveNumberColor = {1.0f, 1.0f, 1.0f, 1.0f};

	// ウェーブゲージの背景
	std::unique_ptr<KamataEngine::Sprite> waveGaugeBackgroundSprite_;

	// 左側のゲージ
	std::unique_ptr<KamataEngine::Sprite> waveGaugeLeftSprite_;

	// 右側のゲージ
	std::unique_ptr<KamataEngine::Sprite> waveGaugeRightSprite_;

	// ゲージの大きさ
	static inline constexpr float kWaveGaugeWidth = 600.0f;
	static inline constexpr float kWaveGaugeHeight = 16.0f;

	// ゲージの表示位置（左上）
	static inline const KamataEngine::Vector2 kWaveGaugePosition = {(kScreenWide - kWaveGaugeWidth) / 2.0f, 60.0f};

	// ゲージの色
	static inline const KamataEngine::Vector4 kWaveGaugeColor = {1.0f, 0.8f, 0.1f, 1.0f};

	//============================================================
	// ウェーブ開始アニメーション
	//============================================================

	// 前回確認したウェーブ番号
	int previousWave_ = 0;

	// アニメーション経過時間
	float waveAnimationTimer_ = 0.0f;

	// 中央で静止する時間
	static inline constexpr float kWaveWaitDuration = 1.0f;

	// 上部へ移動する時間
	static inline constexpr float kWaveMoveDuration = 0.2f;

	// 1フレームの時間
	static inline constexpr float kWaveAnimationDeltaTime = 1.0f / 60.0f;

	// 開始位置
	static inline const KamataEngine::Vector2 kWaveStartPosition = {800.0f, kScreenHeight / 2.0f};

	// 開始時の文字サイズ
	static inline constexpr float kWaveStartFontSize = 100.0f;

	// 現在の描画位置
	KamataEngine::Vector2 waveNumberPosition_ = kWaveStartPosition;

	// 現在の文字サイズ
	float waveNumberFontSize_ = kWaveStartFontSize;

	// ウェーブ開始アニメーションを更新
	void UpdateWaveAnimation();


	//============================================================
	// 敵数表示
	//============================================================

	// 残り敵数の表示位置
	static inline const KamataEngine::Vector2 kEnemyCountPosition = {450.0f, 10.0f};

	// 総敵数の表示位置
	static inline const KamataEngine::Vector2 kEnemyTotalCountPosition = {560.0f, 10.0f};

	// 敵数の文字サイズ
	static inline constexpr float kEnemyCountFontSize = 32.0f;

	// 敵数の文字色
	static inline const KamataEngine::Vector4 kEnemyCountColor = {0.95f, 0.01f, 0.01f, 1.0f};


	//============================================================
	// 操作方法表示
	//============================================================

	// WSD操作スプライトの表示位置
	static inline const KamataEngine::Vector2 kWSDPosition = {120.0f, kScreenHeight - 72.0f};

	// スペースキー操作のスプライト表示位置
	static inline const KamataEngine::Vector2 kSpacePosition = {30.0f + (kScreenWide / 2.0f), kScreenHeight - 72.0f};

	// WSD操作のスプライト
	std::unique_ptr<KamataEngine::Sprite> wsdSprite_;

	// スペースキー操作のスプライト
	std::unique_ptr<KamataEngine::Sprite> spaceSprite_;
};
