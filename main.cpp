#include <Windows.h>
#include "KamataEngine.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	//初期化
	//KamataEngine
	KamataEngine::Initialize(L"TD2_Yumiya");


	while (true) {
		if (KamataEngine::Update()) {
			break;
		}
	
	}


	KamataEngine::Finalize();

	return 0;
}
