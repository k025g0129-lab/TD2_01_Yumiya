#include "BulletTrailParticles.h"

#include "Matrix4x4Util.h"
#include "ModelManager.h"
#include "Vector3Util.h"

#include <algorithm>
#include <cassert>
#include <cmath>

using namespace KamataEngine;

void BulletTrailParticles::Initialize(Camera* camera) {

	assert(camera);

	camera_ = camera;

	model_ = ModelManager::GetInstance()->Load("deathParticle", true);

	assert(model_);

	trailParticles_.clear();
	freeParticles_.clear();
}

void BulletTrailParticles::Emit(const Vector3& position, const Vector3& velocity, int chargeStep, int emissionCount) {

	// チャージ段階を配列の範囲に制限
	chargeStep = std::clamp(chargeStep, 0, 2);

	// チャージ段階に応じた生成数
	int particleCount = kParticleCountTable[chargeStep];

	// 矢の速度から進行方向を計算
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

	if (speed <= 0.0f) {
		return;
	}

	Vector3 direction = {velocity.x / speed, velocity.y / speed, 0.0f};

	// 進行方向と垂直なベクトル
	Vector3 perpendicular = {-direction.y, direction.x, 0.0f};

	// 粒子生成
	for (int i = 0; i < particleCount; ++i) {

		std::unique_ptr<TrailParticle> particle;

		// 再利用できる粒子がある場合
		if (!freeParticles_.empty()) {

			// 待機中の粒子を取得
			particle = std::move(freeParticles_.back());

			// 待機リストから取り除く
			freeParticles_.pop_back();

		} else {

			// 新しい粒子を生成
			particle = std::make_unique<TrailParticle>();

			// GPUリソースの初期化は初回のみ
			particle->worldTransform.Initialize();
			particle->objectColor.Initialize();
		}

		// 粒子の経過時間を初期化
		particle->elapsedTime = 0.0f;

		// 横方向に分散する割合
		float spreadRate = 0.0f;

		if (particleCount > 1) {
			spreadRate = static_cast<float>(i) / static_cast<float>(particleCount - 1) * 2.0f - 1.0f;
		}

		// 発生ごとの揺らぎ
		float offset = std::sin(static_cast<float>(emissionCount) * 1.3f + static_cast<float>(i) * 2.1f) * 0.03f;

		float lateralOffset = spreadRate * kParticleSpawnWidth + offset;

		// 矢の後方に配置
		particle->worldTransform.translation_ = position - direction * 0.4f + perpendicular * lateralOffset;

		// 初期サイズ
		particle->worldTransform.scale_ = {kParticleScale, kParticleScale, kParticleScale};

		// 後方に流れながらわずかに拡散
		particle->velocity = direction * -kParticleTrailSpeed + perpendicular * (spreadRate * kParticleSpreadSpeed);

		// 生成時のチャージ段階の色を保存
		particle->color = kParticleColorTable[chargeStep];

		// ワールド変換更新
		WorldTransformUpdate(particle->worldTransform);

		trailParticles_.push_back(std::move(particle));
	}
}

void BulletTrailParticles::Update() {

	for (auto it = trailParticles_.begin(); it != trailParticles_.end();) {

		TrailParticle* particle = it->get();

		// 経過時間
		particle->elapsedTime += 1.0f / 60.0f;

		// 寿命が終わった粒子を削除
		if (particle->elapsedTime >= kParticleDuration) {

			// 寿命が終わった粒子を再利用リストへ移動
			freeParticles_.push_back(std::move(*it));

			// 使用中の粒子リストから削除
			it = trailParticles_.erase(it);

			continue;
		}

		// 移動
		particle->worldTransform.translation_ += particle->velocity;

		// 寿命に合わせて縮小
		float scaleRate = 1.0f - particle->elapsedTime / kParticleDuration;

		float scale = kParticleScale * scaleRate;

		particle->worldTransform.scale_ = {scale, scale, scale};

		WorldTransformUpdate(particle->worldTransform);

		++it;
	}
}

void BulletTrailParticles::Draw() {

	// 粒子ごとに描画
	for (const std::unique_ptr<TrailParticle>& particle : trailParticles_) {

		// 寿命に応じた透明度
		float alpha = 1.0f - particle->elapsedTime / kParticleDuration;

		alpha = std::clamp(alpha, 0.0f, 1.0f);

		// 粒子固有の色を使用
		Vector4 color = particle->color;
		color.w *= alpha;

		// 粒子専用の色情報を更新
		particle->objectColor.SetColor(color);

		// 粒子を描画
		model_->Draw(particle->worldTransform, *camera_, &particle->objectColor);
	}
}