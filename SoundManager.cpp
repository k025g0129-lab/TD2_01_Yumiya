#include "SoundManager.h"
#include <algorithm>

using namespace KamataEngine;

SoundManager* SoundManager::GetInstance() {
	static SoundManager instance;

	return &instance;
}

void SoundManager::Initialize() {
	Audio* audio = Audio::GetInstance();

	//============================================================
	// BGM
	//============================================================

	bgmSoundHandles_[static_cast<size_t>(BgmType::kTitle)] = audio->LoadWave("sound/bgm/title.mp3");

	bgmSoundHandles_[static_cast<size_t>(BgmType::kGame)] = audio->LoadWave("sound/bgm/game.mp3");

	bgmSoundHandles_[static_cast<size_t>(BgmType::kClear)] = audio->LoadWave("sound/bgm/clear.mp3");


	//============================================================
	// SE
	//============================================================

	seSoundHandles_[static_cast<size_t>(SeType::kCharge)] = audio->LoadWave("sound/se/charge.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kHit)] = audio->LoadWave("sound/se/hit.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kEnemyDamage)] = audio->LoadWave("sound/se/enemyDamage.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kPlayerDamage)] = audio->LoadWave("sound/se/playerDamage.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kShot1)] = audio->LoadWave("sound/se/shot1.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kShot2)] = audio->LoadWave("sound/se/shot2.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kShot3)] = audio->LoadWave("sound/se/shot3.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kMenuConfirm)] = audio->LoadWave("sound/se/menu_confirm.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kPlayerDeath)] = audio->LoadWave("sound/se/playerDeath.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kEnemyDeath)] = audio->LoadWave("sound/se/enemyDeath.mp3");

	seSoundHandles_[static_cast<size_t>(SeType::kWave)] = audio->LoadWave("sound/se/wave.mp3");
}

void SoundManager::PlayBgm(BgmType type) {
	Audio* audio = Audio::GetInstance();

	//============================================================
	// 同じBGMが現在再生中なら何もしない
	//============================================================

	if (currentBgm_ == type && bgmVoiceHandle_ != 0 && audio->IsPlaying(bgmVoiceHandle_)) {
		return;
	}

	// 現在のBGMを停止
	StopBgm();

	size_t index = static_cast<size_t>(type);

	// 総括音量と個別音量から実際の再生音量を計算
	float volume = kBgmMasterVolume * kBgmIndividualVolumes[index];

	// 現在のBGM音量を保存
	currentBgmVolume_ = volume;

	// フェード状態をリセット
	isBgmFadingOut_ = false;

	bgmVoiceHandle_ = audio->PlayWave(bgmSoundHandles_[index], true, volume);

	currentBgm_ = type;
}

void SoundManager::StopBgm() {
	// BGMを再生していなければ何もしない
	if (currentBgm_ == BgmType::kCount) {
		return;
	}

	// 現在のBGMを停止
	Audio::GetInstance()->StopWave(bgmVoiceHandle_);

	bgmVoiceHandle_ = 0;

	currentBgm_ = BgmType::kCount;

	// フェード状態をリセット
	isBgmFadingOut_ = false;
	bgmFadeTimer_ = 0.0f;
	currentBgmVolume_ = 0.0f;
}

void SoundManager::PlaySe(SeType type) {
	size_t index = static_cast<size_t>(type);

	Audio::GetInstance()->PlayWave(seSoundHandles_[index], false, kSeVolume);
}

void SoundManager::FadeOutBgm(float duration) {

	// BGMが再生されていなければ何もしない
	if (bgmVoiceHandle_ == 0) {
		return;
	}

	// 即座に停止する場合
	if (duration <= 0.0f) {
		StopBgm();
		return;
	}

	// フェード開始時の音量を記録
	bgmFadeStartVolume_ = currentBgmVolume_;

	// フェード時間の設定
	bgmFadeDuration_ = duration;
	bgmFadeTimer_ = 0.0f;

	// フェード開始
	isBgmFadingOut_ = true;
}

void SoundManager::Update() {

	if (!isBgmFadingOut_) {
		return;
	}

	// 経過時間を進める
	bgmFadeTimer_ += kSoundDeltaTime;

	// 進行率
	float t = std::clamp(bgmFadeTimer_ / bgmFadeDuration_, 0.0f, 1.0f);

	// 音量を徐々に減らす
	currentBgmVolume_ = bgmFadeStartVolume_ * (1.0f - t);

	// 現在再生中のBGMに音量を反映
	Audio::GetInstance()->SetVolume(bgmVoiceHandle_, currentBgmVolume_);

	// フェード完了
	if (t >= 1.0f) {
		StopBgm();
	}
}