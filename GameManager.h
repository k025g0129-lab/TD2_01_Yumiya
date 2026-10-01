#pragma once
#include "TitleScene.h"
#include "GameScene.h"
#include "StageManager.h"

class GameManager final {
  public:
	void Initialize();
	void Update();
	void Draw();

	GameManager() = default;
	~GameManager();

	// コピー禁止
	GameManager(const GameManager&) = delete;
	GameManager& operator=(const GameManager&) = delete;


	// ゲーム終了リクエストのゲッター
	bool IsExitRequested() const { return isExitRequested_; }

  private:

	// シーン
	enum class Scene {
		kUnknown = 0,
		kTitle,
		kGame,
	};

	// 現在シーン
	Scene scene_ = Scene::kUnknown;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	// タイトルシーン
	TitleScene* titleScene_ = nullptr;

	// ステージマネージャー
	StageManager* stageManager_ = nullptr;

	// ゲーム終了リクエスト
	bool isExitRequested_ = false;


	// シーン管理
	void SceneManager();

	// シーン更新
	void UpdateScene();

	// 全てのシーンを解放
	void DeleteScene();

	// 全てのシーンを開放し、指定されたシーンに変更する
	void CreateScene(Scene newScene);

	// デバッグ設定ファイルの読み込み
	void LoadDebugSettings();
};