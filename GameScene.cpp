#include "GameScene.h"
#include "Enemy.h"
#include "GuardEffect.h"
#include "HitEffect.h"
#include "MapChipField.h"
#include "Matrix4x4Util.h"
#include "ShieldEnemy.h"
#include "RotateEnemy.h"
#include "BoomerangEnemy.h"
#include "JumpEnemy.h"
#include "StageManager.h"
#include "ModelManager.h"
#include "SoundManager.h"
#include "GameInput.h"
#include <algorithm>

using namespace KamataEngine;

namespace {

enum class EnemySpawnType : uint8_t { 
	kNormal = 0,
	kShield = 1,
	kRotate = 2,
	kBoomerang = 3, 
	kJump = 4, 
};

} // namespace

void GameScene::Initialize() {
	// フェーズ初期化
	phase_ = Phase::kFadeIn;

	// ウェーブ初期化
	currentWave_ = 1;
	waveTimer_ = 0.0f;
	totalEnemyCount_ = 0;

	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->farZ = 1100.0f;
	camera_->Initialize();

	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(camera_.get());
	cameraController_->SetMode(CameraMode::kFollow);

	ModelManager* modelManager = ModelManager::GetInstance();

	// 天球の生成
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(modelManager->Load("SkyDome", true), camera_.get());

	// CLEARモデルの読み込み
	// 仮置きタイトルフォント
	modelClear_ = modelManager->Load("ClearText", true);

	// CLEARモデルのワールド変換を初期化
	worldTransformClear_.Initialize();
	worldTransformClear_.scale_ = {1.5f, 1.5f, 1.5f};

	// CLEARモデルの落下初期化
	clearModelTimer_ = 0.0f;
	isClearModelStarted_ = false;

	// マップチップフィールドの生成
	mapChipField_ = std::make_unique<MapChipField>();

	// 現在のステージデータを取得する
	const StageData& stageData = StageManager::GetInstance()->GetCurrentStageData();

	// ステージファイルパスの生成
	std::string stageFileName = "fieldsData/" + stageData.name + ".csv";

	// ステージファイルの読み込み
	mapChipField_->LoadMapChipCsv(stageFileName);

	// フェード
	fade_ = std::make_unique<Fade>();
	fade_->Initialize();
	fade_->Start(Fade::Status::kFadeIn, 1.0f);

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera>(kMaxWindowWidth, kMaxWindowHeight);
	debugCamera_->SetFarZ(camera_->farZ);

	// ブロック
	modelBlock_ = modelManager->Load("block", true);

	HitEffect::SetModel(modelManager->Load("particle", true));
	HitEffect::SetCamera(camera_.get());

	GuardEffect::SetModel(modelManager->Load("ring", true));
	GuardEffect::SetCamera(camera_.get());

	// 調整項目の登録
	Player::RegisterGlobalVariables();

	// 調整項目の適用
	Player::ApplyGlobalVariables();

	// 矢の軌跡パーティクルを生成
	bulletTrailParticles_ = std::make_unique<BulletTrailParticles>();

	// 初期化
	bulletTrailParticles_->Initialize(camera_.get());

	// フィールドのオブジェクトを生成
	GenerateFieldObjects();

	assert(player_ != nullptr && "プレイヤーがCSV内に配置されていません");

	// ウェーブ1の敵を生成
	SpawnWaveEnemies();

	// 追従カメラをプレイヤーにセット
	cameraController_->SetTarget(player_.get());
	cameraController_->Reset();

	// HUDの生成
	gameHUD = std::make_unique<GameHUD>();
	gameHUD->Initialize(player_.get(), camera_.get(), this);

	// BGM
	SoundManager::GetInstance()->PlayBgm(BgmType::kGame);
}

void GameScene::Update() {
	// フェーズ切り替え
	ChangePhase();

	switch (phase_) {
	case Phase::kFadeIn:
		UpdatePlayPhase();
		fade_->Update();
		break;

	case Phase::kPlay:
		UpdatePlayPhase();

		// プレイヤーが生存している場合のみウェーブ更新
		if (!player_->IsDead()) {
			UpdateWave();
		}
		break;

	case Phase::kDeath:
		UpdateDeathPhase();
		break;

	case Phase::kClear:
		UpdateClearPhase();
		break;

	case Phase::kFadeOut:
		fade_->Update();
		break;
	}

	// パーティクル更新
	bulletTrailParticles_->Update();

	// 通常のカメラ更新が止まっている間もシェイクを更新
	if (phase_ == Phase::kDeath || phase_ == Phase::kFadeOut || (phase_ == Phase::kClear && !isClearCameraStarted_)) {
		cameraController_->UpdateShake();
	}

	gameHUD->Update();

#ifdef _DEBUG

	ImGui::Begin("Debug");

	// リロードボタン
	if (ImGui::Button("Reload")) {
		nextScene_ = SCENE::kGame;
	}

	// 追従カメラ起動ボタン
	if (ImGui::Button("Follow Camera")) {
		cameraController_->SetMode(CameraMode::kFollow);
	}

	// 強制スクロール起動ボタン
	if (ImGui::Button("Forced Scroll Camera")) {
		cameraController_->SetMode(CameraMode::kForcedScroll);
	}

	ImGui::End();

#endif // _DEBUG
}

void GameScene::Draw() {
	skydome_->Draw();

	Model::PreDraw();

	for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (const auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			modelBlock_->Draw(*worldTransformBlock, *camera_);
		}
	}

	Model::PostDraw();

	// CLEARモデルの描画
	if (isClearModelStarted_) {

		Model::PreDraw();

		modelClear_->Draw(worldTransformClear_, *camera_);

		Model::PostDraw();
	}


	player_->Draw();

	for (const auto& enemy : enemies_) {
		enemy->Draw();
	}

	Model::PreDraw();

	bulletTrailParticles_->Draw();

	Model::PostDraw();

	// ヒットエフェクトの描画
	DrawHitEffects();

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// クリアカメラ接近開始前まではHUDを表示
	if (!isClearCameraStarted_) {
		gameHUD->Draw();
	}

	if (fade_) {
		fade_->Draw();
	}
}

void GameScene::GenerateFieldObjects() {
	// 要素数
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVertical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数の設定
	worldTransformBlocks_.resize(numBlockVertical);
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		// 1列の要素数の設定
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// 生成
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			Vector3 mapPosition = mapChipField_->GetMapChipPositionByIndex(j, i);

			switch (mapChipField_->GetMapChipTypeByIndex(j, i)) {
				// ブロック
			case MapChipType::kBlock: {
				worldTransformBlocks_[i][j] = std::make_unique<WorldTransform>();

				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_ = mapPosition;
				break;
			}

				// プレイヤー
			case MapChipType::kPlayer: {
				// 既にプレイヤーが生成されているなら停止
				assert(player_ == nullptr && "自キャラを二重に配置しようとしています");

				player_ = std::make_unique<Player>();
				player_->SetMapChipField(mapChipField_.get());
				player_->SetCameraController(cameraController_.get());
				player_->SetBulletTrailParticles(bulletTrailParticles_.get());
				player_->Initialize(camera_.get(), mapPosition);

				break;
			}
			}

			// ブロックを横に伸ばす
			if (mapChipField_->GetMapChipTypeByIndex(0, i) == MapChipType::kBlock) {
				worldTransformBlocks_[i][j] = std::make_unique<WorldTransform>();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_ = mapPosition;
			}
		}
	}
}

void GameScene::GenerateEnemy(Vector3& position, uint8_t type) {
	switch (static_cast<EnemySpawnType>(type)) {
		// 歩行敵の生成
	case EnemySpawnType::kNormal: {
		auto newEnemy = std::make_unique<Enemy>();
		newEnemy->Initialize(camera_.get(), position);
		newEnemy->SetGameScene(this);
		enemies_.push_back(std::move(newEnemy));
		break;
	}

		// 盾敵の生成
	case EnemySpawnType::kShield: {

		//ShieldEnemy* newShieldEnemy = new ShieldEnemy();
		//newShieldEnemy->Initialize(mShieldEnemyModel_, &camera_, position);
		//newShieldEnemy->SetGameScene(this);
		//enemies_.push_back(newShieldEnemy);

		break;
	}


		// 回転敵の生成
	case EnemySpawnType::kRotate: {

		auto newRotateEnemy = std::make_unique < RotateEnemy>();
		newRotateEnemy->Initialize(camera_.get(), position);
		newRotateEnemy->SetGameScene(this);
		enemies_.push_back(std::move(newRotateEnemy));

		break;
	}

	// ブーメラン敵の生成
	case EnemySpawnType::kBoomerang: {

		auto newBoomerangEnemy = std::make_unique<BoomerangEnemy>();
		newBoomerangEnemy->Initialize(camera_.get(), position);
		newBoomerangEnemy->SetGameScene(this);
		enemies_.push_back(std::move(newBoomerangEnemy));

		break;
	}

	case EnemySpawnType::kJump:
			
		auto newJumpEnemy = std::make_unique<JumpEnemy>();
		newJumpEnemy->SetMapChipField(mapChipField_.get());
		newJumpEnemy->Initialize(camera_.get(), position);
		newJumpEnemy->SetGameScene(this);
		enemies_.push_back(std::move(newJumpEnemy));

		break;

	}
}

GameScene::GameScene() {}

GameScene::~GameScene() {
	effects_.clear();
	worldTransformBlocks_.clear();
}

void GameScene::CheckAllCollisions() {
	// プレイヤーのあたり判定
	AABB playerAABB = player_->GetAABB();

	// プレイヤーの矢を取得
	const std::vector<PlayerBullet*>& bullets = player_->GetBullets();

	// 敵とプレイヤーのあたり判定
	for (const auto& enemy : enemies_) {

		// あたり判定無効の敵との処理はスキップ
		if (enemy->IsCollisionDisabled()) {
			continue;
		}

		if (IsCollision(playerAABB, enemy->GetAABB())) {
			player_->OnCollision();
			enemy->OnCollision();
		}

		// 敵の投擲物とプレイヤーの当たり判定

		// ブーメランの敵
		BoomerangEnemy* boomerangEnemy = dynamic_cast<BoomerangEnemy*>(enemy.get());

		// ブーメランの敵かを確認
		if (boomerangEnemy != nullptr) {

			// ブーメランが投げられているなら
			if (boomerangEnemy->GetIsBoomerangFlying()) {

				// プレイヤーとブーメランの接触判定
				if (IsCollision(playerAABB, boomerangEnemy->GetBoomerangAABB())) {
					player_->OnCollision();
				}
			}
		}

		// 矢と敵のあたり判定
		// 全ての矢を確認
		for (PlayerBullet* bullet : bullets) {

			// この敵に命中済みなら判定しない
			if (bullet->HasHitEnemy(enemy.get())) {
				continue;
			}

			// 弾のOBBを取得
			OBB bulletOBB = bullet->GetOBB();

			// 弾と敵の接触判定
			if (IsCollision(enemy->GetAABB(), bulletOBB)) {
				bullet->OnCollision(enemy.get());
				enemy->OnCollisionBullet(bullet->GetAttack());
			}
		}
	}
}

void GameScene::UpdatePlayPhase() {
	// 強制スクロール中だけ、画面端挟まれ死亡判定を有効にする
	player_->SetScreenSqueezeDeathEnabled(cameraController_->GetMode() == CameraMode::kForcedScroll);

	player_->Update();

	for (const auto& enemy : enemies_) {
		enemy->Update();
	}

	// 死んだ敵を削除
	enemies_.remove_if([](const std::unique_ptr<BaseEnemy>& enemy) {
		return enemy->IsDead();
		}
	);

	skydome_->Update();

#ifdef _DEBUG

	// Cを押すとデバッグカメラの有効化フラグをトグル
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

#endif // _DEBUG

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();

		// デバッグカメラが有効ならデバッグカメラのビュー行列とプロジェクション行列をカメラにコピーする
		camera_->matView = debugCamera_->GetCamera().matView;
		camera_->matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		camera_->TransferMatrix();

	} else {
		// ビュープロジェクション行列の更新と転送
		cameraController_->Update();
	}

	// ブロックの更新
	for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (const auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	// あたり判定の更新
	CheckAllCollisions();

	// ヒットエフェクトの更新
	UpdateHitEffects();
}

void GameScene::UpdateDeathPhase() {
	player_->Update();

	for (const auto& enemy : enemies_) {
		enemy->Update();
	}

	skydome_->Update();

	if (deathParticles_) {
		deathParticles_->Update();
	}

	for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (const auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			WorldTransformUpdate(*worldTransformBlock);
		}
	}

	// ヒットエフェクトの更新
	UpdateHitEffects();
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			fade_->Stop();
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kPlay:
		if (player_->IsDead()) {
			// デス演出フェーズに切り替え
			phase_ = Phase::kDeath;

			// 自キャラの座標を取得
			Vector3 deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生
			deathParticles_ = std::make_unique<DeathParticles>();
			deathParticles_->Initialize(camera_.get(), deathParticlesPosition);
		}
		break;

	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::kFadeOut, 1.0f);
		}
		break;

	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			nextScene_ = SCENE::kTitle;
		}
		break;
	}
}

void GameScene::CreateHitEffect(const Vector3& position) {
	effects_.push_back(std::unique_ptr<BaseEffect>(HitEffect::Create(position)));
}

void GameScene::CreateGuardEffect(const Vector3& position) {
	effects_.push_back(std::unique_ptr<BaseEffect>(GuardEffect::Create(position)));
}

void GameScene::UpdateHitEffects() {
	for (const auto& effect : effects_) {
		effect->Update();
	}

	effects_.remove_if([](const std::unique_ptr<BaseEffect>& effect) {
		return effect->IsDead();
		}
	);
}

void GameScene::DrawHitEffects() {
	for (const auto& effect : effects_) {
		effect->Draw();
	}
}

void GameScene::UpdateWave() {

	// ウェーブ経過時間を更新
	waveTimer_ += kWaveDeltaTime;

	// 制限時間に達するか敵が全滅すれば終了
	if (waveTimer_ < kWaveDuration && !enemies_.empty()) {
		return;
	}

	// 次のウェーブへ進行
	AdvanceWave();
}

void GameScene::AdvanceWave() {
	// 最終ウェーブを突破したらクリア
	if (currentWave_ >= kFinalWave) {
		StartClear();
		return;
	}

	// ウェーブ開始SE
	SoundManager::GetInstance()->PlaySe(SeType::kWave);

	// シェイク
	cameraController_->StartShake(0.5f, 10);

	// 現在のウェーブを終了
	ClearWave();

	// ウェーブ番号を進める
	currentWave_++;

	// 経過時間を初期化
	waveTimer_ = 0.0f;

	// 次のウェーブの敵を生成
	SpawnWaveEnemies();
}

void GameScene::ClearWave() {

	// プレイヤーの弾を全削除
	player_->ClearBullets();

	// 敵を全削除
	enemies_.clear();

	// 残弾数を全回復
	player_->ReloadAmmo();
}

void GameScene::SpawnWaveEnemies() {

	// 現在のステージデータを取得
	const StageData& stageData = StageManager::GetInstance()->GetCurrentStageData();

	// ウェーブごとのCSVファイルパスを生成
	std::string waveFileName = "fieldsData/" + stageData.name + "_wave" + std::to_string(currentWave_) + ".csv";

	// 敵配置用のマップチップフィールド
	MapChipField waveField;

	// ウェーブ配置CSVを読み込む
	waveField.LoadMapChipCsv(waveFileName);

	// CSVの縦横サイズを取得
	uint32_t numBlockVertical = waveField.GetNumBlockVertical();
	uint32_t numBlockHorizontal = waveField.GetNumBlockHorizontal();

	// 敵配置データを走査
	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

			// 敵以外のマップチップは無視
			if (waveField.GetMapChipTypeByIndex(j, i) != MapChipType::kEnemy) {
				continue;
			}

			// 配置座標を取得
			Vector3 enemyPosition = waveField.GetMapChipPositionByIndex(j, i);

			// 敵の種類を取得
			uint8_t enemyType = waveField.GetMapChipSubIDByIndex(j, i);

			// 敵を生成
			GenerateEnemy(enemyPosition, enemyType);
		}
	}

	// ウェーブ開始時の総敵数を記録
	totalEnemyCount_ = static_cast<int>(enemies_.size());
}

void GameScene::StartClear() {

	// クリアフェーズに移行
	phase_ = Phase::kClear;

	// 敵と弾を削除
	ClearWave();

	// クリア演出の初期化
	clearTimer_ = 0.0f;
	isClearCameraStarted_ = false;

	// ステージBGMのフェードアウト
	SoundManager::GetInstance()->FadeOutBgm(kClearWaitDuration);
}

void GameScene::UpdateClearPhase() {

	// クリア演出の経過時間を更新
	clearTimer_ += kWaveDeltaTime;

	//============================================================
	// クリア直後の待機
	//============================================================

	if (clearTimer_ < kClearWaitDuration) {
		return;
	}

	//============================================================
	// カメラ接近開始
	//============================================================

	if (!isClearCameraStarted_) {

		// プレイヤーのクリアモーション開始
		player_->RequestClearBehavior();

		// クリアカメラへ切り替え
		cameraController_->StartClearApproach();

		// 開始済みとして記録
		isClearCameraStarted_ = true;

		// クリアBGMの再生
		SoundManager::GetInstance()->PlayBgm(BgmType::kClear);
	}

	// カメラを更新
	cameraController_->Update();

	// クリアモーション更新
	player_->Update();

	//============================================================
	// 接近完了後
	//============================================================

	// カメラ接近中は何もしない
	if (!cameraController_->IsClearApproachFinished()) {
		return;
	}

	// CLEARモデルの落下開始
	if (!isClearModelStarted_) {
		StartClearModelDrop();
	}

	// CLEARモデルの落下更新
	UpdateClearModelDrop();

	// 落下が終わるまで入力を受け付けない
	if (clearModelTimer_ < kClearModelDuration) {
		return;
	}

	// Space入力でタイトルへのフェードアウト開始
	if (GameInput::GetInstance()->IsTrigger(GameAction::kUiConfirm)) {

		phase_ = Phase::kFadeOut;

		fade_->Start(Fade::Status::kFadeOut, 1.0f);
	}
}

void GameScene::StartClearModelDrop() {

	// プレイヤーの位置を取得
	Vector3 playerPosition = player_->GetWorldPosition();

	// 落下開始位置
	clearModelStartPosition_ = playerPosition;
	clearModelStartPosition_.y += kClearModelStartHeight;

	// 落下終了位置
	clearModelEndPosition_ = playerPosition;
	clearModelEndPosition_.y += kClearModelEndHeight;

	// モデルを画面上部に配置
	worldTransformClear_.translation_ = clearModelStartPosition_;

	// ワールド行列更新
	WorldTransformUpdate(worldTransformClear_);

	// 落下タイマー初期化
	clearModelTimer_ = 0.0f;

	// 落下開始
	isClearModelStarted_ = true;
}

void GameScene::UpdateClearModelDrop() {

	// 落下経過時間を更新
	clearModelTimer_ += kWaveDeltaTime;

	// 進行率を取得
	float t = std::clamp(clearModelTimer_ / kClearModelDuration, 0.0f, 1.0f);

	// EaseOutBack
	constexpr float kOvershoot = 1.70158f;

	float easedT = 1.0f + (kOvershoot + 1.0f) * std::pow(t - 1.0f, 3.0f) + kOvershoot * std::pow(t - 1.0f, 2.0f);

	// モデルの落下位置を補間
	worldTransformClear_.translation_.x = std::lerp(clearModelStartPosition_.x, clearModelEndPosition_.x, easedT);
	worldTransformClear_.translation_.y = std::lerp(clearModelStartPosition_.y, clearModelEndPosition_.y, easedT);
	worldTransformClear_.translation_.z = std::lerp(clearModelStartPosition_.z, clearModelEndPosition_.z, easedT);

	// ワールド行列更新
	WorldTransformUpdate(worldTransformClear_);
}