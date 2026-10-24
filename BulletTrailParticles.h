#pragma once

#include "KamataEngine.h"
#include <memory>
#include <vector>

class BulletTrailParticles {
public:
	// 初期化
	void Initialize(KamataEngine::Camera* camera);

	// 更新
	void Update();

	// 描画（Model::PreDraw中に呼び出す）
	void Draw();

	// 粒子生成
	void Emit(const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity, int chargeStep, int emissionCount);

private:
	// 粒子1個分の情報
	struct TrailParticle {
		KamataEngine::WorldTransform worldTransform;
		KamataEngine::Vector3 velocity = {};
		KamataEngine::Vector4 color = {};
		float elapsedTime = 0.0f;
	};

	// 発生した粒子
	std::vector<std::unique_ptr<TrailParticle>> trailParticles_;

	// パーティクルモデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 描画色
	KamataEngine::ObjectColor objectColor_;

	// チャージ段階別の発生数
	static inline constexpr int kParticleCountTable[] = {2, 5, 10};

	// チャージ段階別の色
	static inline const KamataEngine::Vector4 kParticleColorTable[] = {
		{0.7f, 0.7f, 0.7f, 1.0f},
		{1.0f, 0.3f, 0.2f, 1.0f},
		{1.0f, 0.8f, 0.1f, 1.0f}
	};

	// 存続時間
	static inline constexpr float kParticleDuration = 0.5f;

	// 初期サイズ
	static inline constexpr float kParticleScale = 0.3f;

	// 横方向への拡散速度
	static inline constexpr float kParticleSpreadSpeed = 0.01f;

	// 後方へ流れる速度
	static inline constexpr float kParticleTrailSpeed = 0.02f;

	// 発生位置の横方向の広がり
	static inline constexpr float kParticleSpawnWidth = 0.01f;
};