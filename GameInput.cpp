#include "GameInput.h"

using namespace KamataEngine;

GameInput* GameInput::GetInstance() {
	static GameInput instance;

	return &instance;
}

void GameInput::Update() {
	preKeys_ = keys_;
	keys_ = Input::GetInstance()->GetAllKey();
}

bool GameInput::IsPress(GameAction action) { return IsPressedRaw(keys_, action); }

bool GameInput::IsTrigger(GameAction action) {
	bool nowKey = IsPressedRaw(keys_, action);
	bool preKey = IsPressedRaw(preKeys_, action);

	return nowKey && !preKey;
}

bool GameInput::IsRelease(GameAction action) {
	bool nowKey = IsPressedRaw(keys_, action);
	bool preKey = IsPressedRaw(preKeys_, action);

	return !nowKey && preKey;
}

bool GameInput::IsPressedRaw(std::array<BYTE, 256> keys, GameAction action) {
	switch (action) {
	case GameAction::kDirectionDown:
		return keys[DIK_S];

	case GameAction::kDirectionUp:
		return keys[DIK_W];

	case GameAction::kDirectionRight:
		return keys[DIK_D];

	case GameAction::kAttack:
		return keys[DIK_SPACE];

	case GameAction::kUiConfirm:
		return keys[DIK_SPACE];

	default:
		return false;
	}
}
