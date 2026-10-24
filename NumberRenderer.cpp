#include "NumberRenderer.h"
#include <algorithm>
#include <string>
#include <cmath>

using namespace KamataEngine;

void NumberRenderer::Initialize() {
	//============================================================
	// 数字Sprite
	//============================================================

	for (size_t characterIndex = 0; characterIndex < kMaxCharacterCount; ++characterIndex) {
		for (size_t number = 0; number < 10; ++number) {
			std::string texturePath = "font/ui_font_number_" + std::to_string(number) + ".png";

			uint32_t textureHandle = TextureManager::Load(texturePath);

			numberSprites_[characterIndex][number] = Sprite::Create(textureHandle, {0.0f, 0.0f});
		}
	}

	//============================================================
	// % Sprite
	//============================================================

	uint32_t percentTextureHandle = TextureManager::Load("font/ui_font_percent.png");

	for (size_t characterIndex = 0; characterIndex < kMaxCharacterCount; ++characterIndex) {
		percentSprites_[characterIndex] = Sprite::Create(percentTextureHandle, {0.0f, 0.0f});
	}

	//============================================================
	// - Sprite
	//============================================================

	uint32_t minusTextureHandle = TextureManager::Load("font/ui_font_minus.png");

	for (size_t characterIndex = 0; characterIndex < kMaxCharacterCount; ++characterIndex) {
		minusSprites_[characterIndex] = Sprite::Create(minusTextureHandle, {0.0f, 0.0f});
	}

	nextCharacterIndex_ = 0;
}

void NumberRenderer::DrawNumber(int32_t value, const Vector2& position, float fontSize, const Vector4& color, bool drawPercent) {
	// 1文字ごとの横間隔
	float characterInterval = fontSize * kCharacterIntervalRate;

	Vector2 drawPosition = position;

	// 負の数かどうか
	bool isNegative = (value < 0);

	//============================================================
	// -(マイナス記号)
	//============================================================
	if (isNegative) {
		if (nextCharacterIndex_ >= kMaxCharacterCount) {
			return;
		}

		Sprite* minusSprite = minusSprites_[nextCharacterIndex_];
		minusSprite->SetPosition(drawPosition);
		minusSprite->SetSize({fontSize, fontSize});
		minusSprite->SetColor(color);
		minusSprite->Draw();

		++nextCharacterIndex_;
		drawPosition.x += characterInterval;
	}

	// 絶対値を決める
	uint32_t absValue = static_cast<uint32_t>(std::abs(value));
	// 数字を文字列化
	std::string numberText = std::to_string(absValue);

	//============================================================
	// 数字
	//============================================================

	for (char character : numberText) {
		// 描画可能数を超えた場合
		if (nextCharacterIndex_ >= kMaxCharacterCount) {
			return;
		}

		// '0'～'9'を0～9へ変換
		size_t number = static_cast<size_t>(character - '0');

		// この文字専用のSpriteを取得
		Sprite* numberSprite = numberSprites_[nextCharacterIndex_][number];

		numberSprite->SetPosition(drawPosition);

		numberSprite->SetSize({fontSize, fontSize});

		numberSprite->SetColor(color);

		numberSprite->Draw();

		// 次の文字Spriteを使用
		++nextCharacterIndex_;

		drawPosition.x += characterInterval;
	}

	//============================================================
	// %
	//============================================================

	if (drawPercent) {
		if (nextCharacterIndex_ >= kMaxCharacterCount) {
			return;
		}

		Sprite* percentSprite = percentSprites_[nextCharacterIndex_];

		percentSprite->SetPosition(drawPosition);

		percentSprite->SetSize({fontSize, fontSize});

		percentSprite->SetColor(color);

		percentSprite->Draw();

		++nextCharacterIndex_;
	}

	//=============================================================
	// -
	//=============================================================
}

void NumberRenderer::DrawNumberRightAligned(int32_t value, const Vector2& position, size_t maxDigitCount, float fontSize, const Vector4& color, bool drawPercent) {

	// 実際の桁数を取得
	std::string numberText = std::to_string(value);

	size_t digitCount = numberText.size();

	// 1文字ごとの横間隔
	float characterInterval = fontSize * kCharacterIntervalRate;

	// 描画開始位置
	Vector2 drawPosition = position;

	// 最大桁数より実際の桁数が少ない分だけ右へずらす
	if (digitCount < maxDigitCount) {
		size_t emptyDigitCount = maxDigitCount - digitCount;

		drawPosition.x += static_cast<float>(emptyDigitCount) * characterInterval;
	}

	// 通常の数字描画を利用
	DrawNumber(value, drawPosition, fontSize, color, drawPercent);
}

void NumberRenderer::DrawNumberCenterAligned(int32_t value, const Vector2& position, float fontSize, const Vector4& color, bool drawPercent) {

	// 表示する数字を文字列化
	std::string numberText = std::to_string(value);

	// 表示する文字数を取得
	size_t characterCount = numberText.size();

	// %記号を表示する場合は文字数を追加
	if (drawPercent) {
		characterCount++;
	}

	// 1文字ごとの横間隔
	float characterInterval = fontSize * kCharacterIntervalRate;

	// 表示全体の横幅を算出
	float textWidth = (static_cast<float>(characterCount) - 1.0f) * characterInterval + fontSize;

	// 指定された座標を中心とする描画開始位置
	Vector2 drawPosition = position;

	drawPosition.x -= textWidth / 2.0f;

	// 既存の数字描画関数を使用
	DrawNumber(value, drawPosition, fontSize, color, drawPercent);
}

NumberRenderer::~NumberRenderer() {
	for (auto& characterSprites : numberSprites_) {
		for (Sprite* numberSprite : characterSprites) {
			delete numberSprite;
		}
	}

	for (Sprite* percentSprite : percentSprites_) {
		delete percentSprite;
	}

	for (Sprite* minusSprite : minusSprites_) {
		delete minusSprite;
	}
}

void NumberRenderer::BeginFrame() { nextCharacterIndex_ = 0; }