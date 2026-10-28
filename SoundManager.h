#pragma once
#include "KamataEngine.h"
#include <array>
#include <cstddef>
#include <cstdint>

//============================================================
// BGM
//============================================================

enum class BgmType {
	kTitle,
	kGame,
	kClear,

	kCount,
};

//============================================================
// SE
//============================================================

enum class SeType {
	kCharge,
	kShot1,
	kShot2,
	kShot3,
	kHit,
	kEnemyDamage,
	kPlayerDamage,
	kPlayerDeath,
	kEnemyDeath,
	kWave,

	kMenuConfirm,

	kCount,
};

class SoundManager {
public:
	static SoundManager* GetInstance();

	// 音源読み込み
	void Initialize();

	//============================================================
	// BGM
	//============================================================

	void PlayBgm(BgmType type);
	void StopBgm();

	//============================================================
	// SE
	//============================================================

	void PlaySe(SeType type);

	// BGMフェードアウト開始
	void FadeOutBgm(float duration);

	// BGMの更新
	void Update();

private:
	// シングルトンパターンにする
	SoundManager() = default;
	~SoundManager() = default;
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;


	static inline const size_t kBgmCount = static_cast<size_t>(BgmType::kCount);

	static inline const size_t kSeCount = static_cast<size_t>(SeType::kCount);

	// 読み込んだBGM
	std::array<uint32_t, kBgmCount> bgmSoundHandles_{};

	// 読み込んだSE
	std::array<uint32_t, kSeCount> seSoundHandles_{};

	// 現在再生中のBGM
	uint32_t bgmVoiceHandle_ = 0;

	// 現在のBGM
	BgmType currentBgm_ = BgmType::kCount;

	//============================================================
	// 音量
	//============================================================

	// BGM全体音量
	static inline const float kBgmMasterVolume = 0.35f;

	static inline const float kSeVolume = 0.7f;


	// BGMごとの個別音量
	static inline const std::array<float, kBgmCount>
	    kBgmIndividualVolumes = {
	        1.0f, // Title
	        1.0f, // Game
	        1.0f, // Clear
	};


	// BGMフェードアウト中か
	bool isBgmFadingOut_ = false;

	// フェード経過時間
	float bgmFadeTimer_ = 0.0f;

	// フェード時間
	float bgmFadeDuration_ = 0.0f;

	// フェード開始時の音量
	float bgmFadeStartVolume_ = 0.0f;

	// 現在のBGM音量
	float currentBgmVolume_ = 0.0f;

	// 1フレームの時間
	static inline constexpr float kSoundDeltaTime = 1.0f / 60.0f;
};