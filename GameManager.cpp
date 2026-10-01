#include "GameManager.h"
#include "GlobalVariables.h"
#include "GameInput.h"
#include <fstream>
#include <string>

void GameManager::Initialize() {
	stageManager_ = new StageManager();

	// ステージデータファイルを読み込む
	stageManager_->LoadStageDataCsv();

	#ifdef _DEBUG

	LoadDebugSettings();

	#endif // _DEBUG

	// 起動時のグローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();

	// 最初のシーンを生成
	scene_ = Scene::kTitle;
	CreateScene(scene_);
}

void GameManager::Update() {

	// インプットの更新
	GameInput::GetInstance()->Update();

	// シーンの管理
	SceneManager();

	// 現在シーンの更新
	UpdateScene();

	// グローバル変数の更新
	GlobalVariables::GetInstance()->Update();
}

void GameManager::Draw() {
	switch (scene_) {
	case Scene::kTitle:
		if (titleScene_) {
			titleScene_->Draw();
		}
		break;

		// case Scene::kStageSelect:
		//	if (stageSelectScene_) {
		//		stageSelectScene_->Draw();
		//	}
		//	break;

	case Scene::kGame:
		if (gameScene_) {
			gameScene_->Draw();
		}
		break;

		// case Scene::kResult:
		//	if (resultScene_) {
		//		resultScene_->Draw();
		//	}
		//	break;

	case Scene::kUnknown:
	default:
		break;
	}
}

GameManager::~GameManager() {

	// 全てのシーンを解放
	DeleteScene();

	// ステージマネージャーの開放
	delete stageManager_;
	stageManager_ = nullptr;
}

void GameManager::SceneManager() {
	switch (scene_) {
	case Scene::kTitle:
		if (titleScene_ && titleScene_->IsFinished()) {
			// シーン変更
			CreateScene(Scene::kGame);
		}
		break;

	//case Scene::kStageSelect:
	//	if (stageSelectScene_ && stageSelectScene_->IsFinished()) {
	//		// シーン移行
	//		CreateScene(stageSelectScene_->GetNextScene());
	//	}
	//	break;

	case Scene::kGame: {
		if (gameScene_ && gameScene_->IsFinished()) {
			// シーン変更
			CreateScene(Scene::kTitle);

			// ホットリロード
		} else if (gameScene_ && gameScene_->GetReloadRequest()) {
			// グローバル変数の再読み込み
			GlobalVariables::GetInstance()->LoadFiles();

			// 新シーンの生成
			CreateScene(Scene::kGame);
		}
		break;
	}

	//case Scene::kResult_: {
	//	if (resultScene_ && resultScene_->IsFinished()) {
	//		// シーン変更
	//		CreateScene(Scene::kStageSelect_);
	//	}

	//	break;
	//}

	case Scene::kUnknown:
	default:
		break;
	}
}

void GameManager::UpdateScene() {
	switch (scene_) {
	case Scene::kTitle:
		if (titleScene_) {
			titleScene_->Update();
		}
		break;

	//case Scene::kStageSelect:
	//	if (stageSelectScene_) {
	//		stageSelectScene_->Update();
	//	}
	//	break;

	case Scene::kGame:
		if (gameScene_) {
			gameScene_->Update();
		}
		break;

	//case Scene::kResult:
	//	if (resultScene_) {
	//		resultScene_->Update();
	//	}
	//	break;

	case Scene::kUnknown:
	default:
		break;
	}
}

void GameManager::DeleteScene() {
	delete titleScene_;
	titleScene_ = nullptr;

	//delete stageSelectScene_;
	//stageSelectScene_ = nullptr;

	delete gameScene_;
	gameScene_ = nullptr;

	//delete resultScene_;
	//resultScene_ = nullptr;
}

void GameManager::CreateScene(Scene newScene) {// シーンの解放
	DeleteScene();

	scene_ = newScene;

	switch (scene_) {
	case Scene::kTitle:
		// 新シーンの生成と初期化
		titleScene_ = new TitleScene();

		titleScene_->Initialize();

		break;

	//case Scene::kStageSelect:
	//	// タイトルモデルを読み込む
	//	// ステージセレクトでもタイトルと同じ読み込み
	//	modelManager->LoadTitleModels();

	//	// 新シーンの生成と初期化
	//	stageSelectScene = new StageSelectScene();

	//	stageSelectScene->Initialize(stageManager, modelManager);

	//	break;

	case Scene::kGame:
		// 新シーンの生成
		gameScene_ = new GameScene();
		gameScene_->Initialize(stageManager_);

		break;

	//case Scene::kResult:

	//	// 新シーンの生成と初期化
	//	resultScene = new ResultScene();
	//	resultScene->Initialize(scoreManager, rankingManager, stageManager);

	//	break;

	case Scene::kUnknown:
	default:
		break;
	}
}

void GameManager::LoadDebugSettings() {
	std::ifstream file("DebugSettings.ini");

	// 個人用ファイルなので、存在しなくても問題なし
	if (!file.is_open()) {
		return;
	}

	std::string key;
	std::string value;

	while (file >> key >> value) {
		if (key == "InitialStage") {
			stageManager_->SetCurrentStageIndexByName(value);
		}
	}
}
