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

	objectColor_.Initialize();

	trailParticles_.clear();
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

		std::unique_ptr<TrailParticle> particle = std::make_unique<TrailParticle>();

		particle->worldTransform.Initialize();

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

	// 呼び出し元でModel::PreDraw()を実行する
	for (const std::unique_ptr<TrailParticle>& particle : trailParticles_) {

		// 寿命に応じた透明度
		float alpha = 1.0f - particle->elapsedTime / kParticleDuration;

		alpha = std::clamp(alpha, 0.0f, 1.0f);

		// 粒子固有の色を使う
		Vector4 color = particle->color;
		color.w *= alpha;

		objectColor_.SetColor(color);

		Model::PreDraw();

		model_->Draw(particle->worldTransform, *camera_, &objectColor_);

		Model::PostDraw();
	}
}