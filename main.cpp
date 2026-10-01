#include <Windows.h>
#include"KamataEngine.h"
#include "GameManager.h"


using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジンの初期化
	::Initialize(L"LE2C_14_タナカ_コウヘイ");
	
	// DirectXCommonインスタンスの取得
	DirectXCommon* dXCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imGuiManager = ImGuiManager::GetInstance();

	GameManager* gameManager = new GameManager();
	gameManager->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// ImGui受付開始
		imGuiManager->Begin();

		// ゲームの更新
		gameManager->Update();

		// ImGui受付終了
		imGuiManager->End();


		// 描画開始
		dXCommon->PreDraw();

		// ここに描画処理を記述

		// ゲームの描画
		gameManager->Draw();

		// ImGui描画
		imGuiManager->Draw();

		// 描画終了
		dXCommon->PostDraw();

		// ゲーム終了リクエストがあればループを抜ける
		if (gameManager->IsExitRequested()) {
			break;
		}
	}

	//　ゲームの開放
	delete gameManager;
	gameManager = nullptr;

	// エンジンの終了処理
	::Finalize();

	return 0;
}