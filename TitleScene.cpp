#include "TitleScene.h"
#include "Matrix4x4Util.h"
#include "Vector3Util.h"
#include <cassert>
#include <cmath>
#include <numbers>
#include "ModelManager.h"
#include "SoundManager.h"

using namespace KamataEngine;

void TitleScene::Initialize() {
	phase_ = Phase::kFadeIn;
	animationTimer_ = 0.0f;

	// カメラ初期化
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->translation_ = {0.0f, 1.0f, -10.0f};
	camera_->rotation_ = {0.0f, 0.0f, 0.0f};
	camera_->UpdateMatrix();

	ModelManager* modelManager = ModelManager::GetInstance();

	// モデル読み込み
	modelTitle_ = modelManager->Load("TitleText", true);
	modelPlayer_ = modelManager->Load("player", true);
	modelEnemys_[0] = modelManager->Load("RotateEnemy", true);
	modelEnemys_[1] = modelManager->Load("BoomerangEnemy", true);
	modelEnemys_[2] = modelManager->Load("JumpEnemy", true);
	modelEnemys_[3] = modelManager->Load("ThrowingBorn", true);

	// タイトル文字のワールド変換
	worldTransformTitle_.Initialize();
	worldTransformTitle_.translation_ = {0.5f, 0.0f, 0.0f};
	worldTransformTitle_.scale_ = {1.5f, 1.5f, 1.5f};

	// プレイヤーモデルのワールド変換
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_ = {0.0f, -1.0f, 0.0f};
	worldTransformPlayer_.rotation_.y = std::numbers::pi_v<float> / 2.0f;



	for (int i = 0; i < 4; i++) {
		//敵モデルのワールド変換
		worldTransformEnemys_[i].Initialize();
		worldTransformEnemys_[i].translation_ = {float(rand() % 9 - 4), float(rand() % 3 + 2), 0.0f};
		worldTransformEnemys_[i].rotation_.y = std::numbers::pi_v<float> / 2.0f;
		enemysVelocty_[i] = {
		    float(rand() % 5 - 2) / 100.0f,
		     - float(rand() % 6 + 3) / 100.0f,
		};

	}


	// フェード
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::kFadeIn, 1.0f);

	// BGM
	SoundManager::GetInstance()->PlayBgm(BgmType::kTitle);
}

void TitleScene::Update() {
	// アニメーション時間を進める
	animationTimer_ += 1.0f / 60.0f;

	// プレイヤーを上下にふわふわさせる
	float floating = std::sin(2.0f * kPi * animationTimer_) * 0.2f;
	worldTransformPlayer_.translation_.y = -1.0f + floating;

	// プレイヤーをゆっくり回転させる
	worldTransformPlayer_.rotation_.y += 0.02f;

	// タイトル文字も少しだけ上下させる
	float titleFloating = std::sin(2.0f * kPi * animationTimer_ * 0.5f) * 0.1f;
	worldTransformTitle_.translation_.y = 3.0f + titleFloating;


	//敵の落下
	for (int i = 0; i < 4; i++) {
		//worldTransformEnemys_[i].translation_.x += enemysVelocty_[i].x;
		worldTransformEnemys_[i].translation_.y += enemysVelocty_[i].y;


		if (worldTransformEnemys_[i].translation_.y <= -5.0f) {
			worldTransformEnemys_[i].translation_ = {float(rand() % 9 - 4), float(rand() % 5 + 5), 0.0f};
		}

	}


	// ワールド行列更新
	WorldTransformUpdate(worldTransformTitle_);
	WorldTransformUpdate(worldTransformPlayer_);
	for (int i = 0; i < 4; i++) {
		WorldTransformUpdate(worldTransformEnemys_[i]);
	}


	// カメラ行列更新
	camera_->UpdateMatrix();

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::kFadeOut, 1.0f);
			SoundManager::GetInstance()->PlaySe(SeType::kMenuConfirm);
		}
		break;

	case Phase::kFadeOut:
		fade_->Update();

		if (fade_->IsFinished()) {
			nextScene_ = SCENE::kGame;
		}
		break;
	}
}

void TitleScene::Draw() {
	Model::PreDraw();

	if (modelTitle_) {
		modelTitle_->Draw(worldTransformTitle_, *camera_);
	}

	if (modelPlayer_) {
		modelPlayer_->Draw(worldTransformPlayer_, *camera_);
	}

	for (int i = 0; i < 4; i++) {
		if (modelEnemys_[i]) {
			modelEnemys_[i]->Draw(worldTransformEnemys_[i], *camera_);
		}
	
	
	}

	Model::PostDraw();

	if (fade_) {
		fade_->Draw();
	}
}

TitleScene::~TitleScene() {
}
