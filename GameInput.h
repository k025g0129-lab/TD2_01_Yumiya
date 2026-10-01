#pragma once
#include "KamataEngine.h"

// ゲーム内で使用する操作
enum class GameAction {
	kDirectionUp,    // 弓矢の向き 上
	kDirectionDown,  // 弓矢の向き 下
	kDirectionRight, // 弓矢の向き 右
	kAttack, // 通常攻撃

	// 仮置き移動　後で消す
	kMoveUp, // 移動 上
	kMoveDown, // 移動 下
	kMoveRight, // 移動 右
	kMoveLeft,  // 移動 左
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



	// コピーコンストラクタと代入演算子を削除してシングルトンパターンにする
	GameInput(const GameInput&) = delete;
	GameInput& operator=(const GameInput&) = delete;

private:
	// キーの状態を保持する配列
	std::array<BYTE, 256> keys_{};
	std::array<BYTE, 256> preKeys_{};

	// シングルトンパターンにするためのコンストラクタとデストラクタをprivateにする
	GameInput() = default;
	~GameInput() = default;


	bool IsPressedRaw(std::array<BYTE, 256> keys, GameAction action);
};
