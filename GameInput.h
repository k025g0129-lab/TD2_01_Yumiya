#pragma once
#include "KamataEngine.h"

// ゲーム内で使用する操作
enum class GameAction {
	kDirectionUp,    // 弓矢の向き 上
	kDirectionDown,  // 弓矢の向き 下
	kDirectionRight, // 弓矢の向き 右
	kAttack, // 通常攻撃

	kUiConfirm
};

class GameInput final {
public:

	static GameInput* GetInstance();

	// キー情報の更新
	void Update();

	// 押している間
	bool IsPress(GameAction action);

	// 押した瞬間
	bool IsTrigger(GameAction action);

	// 離した瞬間
	bool IsRelease(GameAction action);




private:
	// キーの状態を保持する配列
	std::array<BYTE, 256> keys_{};
	std::array<BYTE, 256> preKeys_{};

	bool IsPressedRaw(std::array<BYTE, 256> keys, GameAction action);

	// シングルトンパターンにするためのコンストラクタとデストラクタをprivateにする
	GameInput() = default;
	~GameInput() = default;
	// コピーコンストラクタと代入演算子を削除してシングルトンパターンにする
	GameInput(const GameInput&) = delete;
	GameInput& operator=(const GameInput&) = delete;
};
