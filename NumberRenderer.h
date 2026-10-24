#pragma once

#include "KamataEngine.h"
#include <array>
#include <cstdint>

class NumberRenderer {
public:
	// 初期化
	void Initialize();

	// 数字を描画
	void DrawNumber(int32_t value, const KamataEngine::Vector2& position, float fontSize, const KamataEngine::Vector4& color, bool drawPercent = false);

	// 最大桁数を基準に右詰めで数字を描画
	void DrawNumberRightAligned(int32_t value, const KamataEngine::Vector2& position, size_t maxDigitCount, float fontSize, const KamataEngine::Vector4& color, bool drawPercent = false);

	// 現在の文字数を基準に中央揃えで数字を描画
	void DrawNumberCenterAligned(int32_t value, const KamataEngine::Vector2& position, float fontSize, const KamataEngine::Vector4& color, bool drawPercent = false);

	~NumberRenderer();

	// 1フレーム分の描画を開始
	void BeginFrame();

private:
	// 1フレームで描画可能な最大文字数
	static inline const size_t kMaxCharacterCount = 64;

	// 各文字位置用の0～9Sprite
	std::array<std::array<KamataEngine::Sprite*, 10>, kMaxCharacterCount> numberSprites_{};

	// 各文字位置用の%Sprite
	std::array<KamataEngine::Sprite*, kMaxCharacterCount> percentSprites_{};

	// 各文字位置用の-sprite
	std::array<KamataEngine::Sprite*, kMaxCharacterCount> minusSprites_{};

	// 次に使用する文字Sprite番号
	size_t nextCharacterIndex_ = 0;

	// 文字間隔倍率
	static inline const float kCharacterIntervalRate = 1.0f;
};