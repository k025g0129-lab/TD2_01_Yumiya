#define NOMINMAX
#include "Player.h"
#include "Ease.h"
#include "GameInput.h"
#include "GlobalVariables.h"
#include "MapChipField.h"
#include "Matrix4x4Util.h"
#include "Vector3Util.h"
#include <algorithm>
#include <assert.h>
#include <numbers>
#include "ModelManager.h"
#include <cmath>
#include"SoundManager.h"

using namespace KamataEngine;

Player::~Player() { ClearBullets(); }

void Player::Initialize(Camera* camera, Vector3 position) {

	ModelManager* modelManager = ModelManager::GetInstance();

	// モデルマネージャーからモデルを取得
	model_ = modelManager->Load("player", true);

	assert(model_);
	assert(camera);


	// 死亡フラグを初期化
	isDead_ = false;
	deathType_ = DeathType::kNone;
	isDeathMotionFinished_ = false;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 初期配置座標
	worldTransform_.translation_ = position;

	// モデルの向きに合わせてY軸回りに90度回転させる
	worldTransform_.rotation_.y = kPi / 2.0f;

	// 描画専用ワールド変換の初期化
	drawWorldTransform_.Initialize();

	// 振動時間の初期化
	chargeShakeTimer_ = 0.0f;

	// 引数で渡されたカメラをメンバ変数に保存
	camera_ = camera;

	// 振る舞いの初期化
	behavior_ = Behavior::kRoot;
	behaviorRequest_ = Behavior::kUnknown;

	BehaviorRootInitialize();

	// ジャンプ通り抜けるフラグ初期化
	isJumpThrough_ = false;

	//通過ポイント初期化
	JumpThroughInitialize();

	// 矢のチャージ時間の初期化
	chargeTime_ = 0;

	// HPの初期化
	HP_ = kMaxHP;

	// モデル色を初期化
	objectColor_.Initialize();

	objectColorValue_ = {1.0f, 1.0f, 1.0f, 1.0f};

	objectColor_.SetColor(objectColorValue_);

	damageColorTimer_ = 0.0f;

	// 照準方向の初期化
	direction_ = Direction::kRight;
	previousAimDirection_ = direction_;

	// 照準回転の初期化
	worldTransform_.rotation_.z = 0.0f;
	aimStartRotationZ_ = 0.0f;
	aimTurnTimer_ = kAimTurnTime;

	// 残弾数の初期化
	ammo_ = kMaxAmmo;
}

void Player::Update() {
	// 弾の更新
	for (int i = static_cast<int>(bullets_.size()) - 1; i >= 0; --i) {

		// 消滅していない矢の更新
		if (!bullets_[i]->IsFinished()) {
			bullets_[i]->Update();
		}

		// 命中した弾、またはカメラ外の弾を削除
		if (bullets_[i]->IsFinished() || !bullets_[i]->GetisInCamera()) {
			delete bullets_[i];
			bullets_.erase(bullets_.begin() + i);
		}
	}

	// 外部からのノックバック要求を処理
	if (isKnockbackRequested_) {
		// 攻撃中のスケールが残らないように戻す
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

		behaviorRequest_ = Behavior::kKnockback;
		isKnockbackRequested_ = false;
	}

	// 振る舞い変更のリクエストが来ていた場合、振る舞いを変更する
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞い変更の反映
		behavior_ = behaviorRequest_;

		// 振る舞いごとの初期化
		switch (behavior_) {
			// kRootとdefaultは同じ処理
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Behavior::kCharge:
			BehaviorChargeInitialize();
			break;

		case Behavior::kKnockback:
			BehaviorKnockInitialize();
			break;

		case Behavior::kSqueezeDeath:
			BehaviorSqueezeDeathInitialize();
			break;
		}

		// 振る舞いリクエストの初期化
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
		// kRootとdefaultは同じ処理
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;

	case Behavior::kCharge:
		BehaviorChargeUpdate();
		break;

	case Behavior::kKnockback:
		BehaviorKnockUpdate();
		break;

	case Behavior::kSqueezeDeath:
		BehaviorSqueezeDeathUpdate();
		break;
	}

	// プレイヤーの照準方向に合わせてモデルを回転
	if (behavior_ == Behavior::kRoot || behavior_ == Behavior::kCharge) {
		ChangeRotationToDirection();
	}

	// 強制スクロール中かつ生存中は画面端補正と挟まれ死亡判定を行う
	if (!isDead_) {
		if (isScreenSqueezeDeathEnabled_ && !isDead_) {
			// 画面端からはみ出さないように補正
			CorrectPositionInScreen();

			// 画面端とブロックに挟まれていたら死亡
			CheckScreenSqueezeDeath();
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒カウントダウンする
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
		    kPi / 2.0f,        // 右
		    kPi * 3.0f / 2.0f, // 左
		};

		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 0.0f から 1.0f に進む補間割合
		float t = 1.0f - std::clamp(turnTimer_ / kTurnTime, 0.0f, 1.0f);

		// イージングをかける
		float easedT = Ease::EaseInOut(t);

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = std::lerp(turnFirstRotationY_, destinationRotationY, easedT);
	}

	// 被弾後無敵を更新
	UpdateDamageInvincibility();

	// 被弾視覚効果を更新
	UpdateDamageVisual();

// 削除予定
#ifdef _DEBUG

	//ImGui::Begin("JumpThrough");

	//// リロードボタン
	//if (isJumpThrough_) {
	//	ImGui::Text("JumpThrough: ON");
	//} else {
	//	ImGui::Text("JumpThrough: OFF");
	//}

	//// 接地判定
	//if (isGround_) {
	//	ImGui::Text("isGround: ON");
	//} else {
	//	ImGui::Text("isGround: OFF");
	//}

	//ImGui::Text("pos.x = %.2f", GetWorldPosition().x);

	//ImGui::Text("worldTransform_.translation_.y = %f , jumpThroughPoint_ = %f", worldTransform_.translation_.y, jumpThroughPoint_[1].yPos_);
	//ImGui::Text("velocity_.y = %f ", velocity_.y);
	//ImGui::Text("chargeKeyTime = %d ", chargeTime_);
	//ImGui::Text("LowerLimitBlockYIndex = %d ", int(mapChipField_->GetLowerLimitBlockYIndex() - uint32_t(1)));
	//ImGui::Text("PlayerYIndex = %d ", int(mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_).yIndex));


	//ImGui::End();

#endif // _DEBUG

	// ワールド変換の更新
	WorldTransformUpdate(worldTransform_);

	// チャージ振動の更新
	UpdateChargeShake();
}

void Player::Draw() {
	// 描画すべきでないなら描画しない
	if (!ShouldDraw()) {
		return;
	}

	// 描画前処理
	Model::PreDraw();

	// 弾の描画
	for (PlayerBullet* bullet : bullets_) {
		if (!bullet->IsFinished()) {
			bullet->Draw();
		}
	}

	//============================================================
	// 被弾無敵中の点滅
	//============================================================

	// 赤表示が終了した後だけ点滅
	if (damageColorTimer_ <= 0.0f && damageInvincibleTimer_ > 0.0f) {
		float blinkTime = std::fmod(damageInvincibleTimer_, kDamageBlinkInterval);

		// 周期の半分は非表示
		if (blinkTime < kDamageBlinkInterval / 2.0f) {
			return;
		}
	}

	// 自機の描画
	model_->Draw(drawWorldTransform_, *camera_, &objectColor_);

	// 描画後処理
	Model::PostDraw();
}

void Player::BehaviorRootInitialize() {}

void Player::BehaviorRootUpdate() {
	// 空中での落下処理
	if (!isGround_) {
		// 落下速度
		velocity_ += Vector3(0.0f, -kGravityAcceleration, 0.0f);

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 方向変更処理
	PlayerDirectionUpdate();

	// 攻撃状態へのリクエスト
	if (GameInput::GetInstance()->IsTrigger(GameAction::kAttack)) {
		if (ammo_ > 0) {
			behaviorRequest_ = Behavior::kCharge;
		} else {
			// 弾切れUI表示リクエストの予定
		}
	}

	//通過ポイント更新
	JumpThroughUpdate();

	ResolveMapChipCollision(velocity_);
}

void Player::JumpThroughInitialize() {

	int jumpThroughPointIndex = 0;

	//ブロックを感知するとジャンプポイントを接地
	for (int i = 0; i < static_cast<int>(mapChipField_->GetNumBlockVertical()); i++) {
		if (mapChipField_->GetMapChipTypeByIndex(0, i) == MapChipType::kBlock) {

			if (jumpThroughPointIndex >= maxJumpThroughPointIndex) {
				break;
			}

			jumpThroughPoint_[jumpThroughPointIndex].yPos_ = mapChipField_->GetMapChipPositionByIndex(0, i).y;

			IsJumpThroughPointOver(&jumpThroughPoint_[jumpThroughPointIndex]);

			// iではなくjumpThroughPointIndexを使用
			jumpThroughPoint_[jumpThroughPointIndex].isPreOver = jumpThroughPoint_[jumpThroughPointIndex].isOver;

			jumpThroughPointIndex++;
		}
	}

	//上限下限位置設定
	jumpUpperLimitsPoint.yPos_ = mapChipField_->GetMapChipPositionByIndex(uint32_t(0), mapChipField_->GetUpperLimitBlockYIndex() - uint32_t(1)).y;
	jumpLowerLimitsPoint.yPos_ = mapChipField_->GetMapChipPositionByIndex(uint32_t(0), mapChipField_->GetLowerLimitBlockYIndex() - uint32_t(1)).y;
	
	IsJumpThroughPointOver(&jumpUpperLimitsPoint);
	IsJumpThroughPointOver(&jumpLowerLimitsPoint);

	// トリガー用
	jumpUpperLimitsPoint.isPreOver = jumpUpperLimitsPoint.isOver;
	jumpLowerLimitsPoint.isPreOver = jumpLowerLimitsPoint.isOver;
}

void Player::JumpThroughUpdate() {

	for (int i = 0; i < maxJumpThroughPointIndex; i++) {

		//通過ポイントより上か下かの処理
		IsJumpThroughPointOver(&jumpThroughPoint_[i]);

		//通過したなら他の場所を透過出来ないように
		if (jumpThroughPoint_[i].isOver != jumpThroughPoint_[i].isPreOver) {
			isJumpThrough_ = false;
		}

		// トリガー用
		jumpThroughPoint_[i].isPreOver = jumpThroughPoint_[i].isOver;
	}

	IsJumpThroughPointOver(&jumpUpperLimitsPoint);
	IsJumpThroughPointOver(&jumpLowerLimitsPoint);

	// 通過したなら他の場所を透過出来ないように
	if (jumpUpperLimitsPoint.isOver != jumpUpperLimitsPoint.isPreOver) {
		isJumpThrough_ = false;
	}

	// トリガー用
	jumpUpperLimitsPoint.isPreOver = jumpUpperLimitsPoint.isOver;
	jumpLowerLimitsPoint.isPreOver = jumpLowerLimitsPoint.isOver;
}

void Player::IsJumpThroughPointOver(JumpThroughPoint* jumpThroughPoint) {

	// 通過ポイントより上か下かの処理
	if (worldTransform_.translation_.y > jumpThroughPoint->yPos_) {
		jumpThroughPoint->isOver = true;

	} else {
		jumpThroughPoint->isOver = false;
	}

}

void Player::BehaviorChargeInitialize() {
}

void Player::BehaviorChargeUpdate() {

	// 空中での落下処理
	if (!isGround_) {
		// 落下速度
		velocity_ += Vector3(0.0f, -kGravityAcceleration, 0.0f);

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 方向変更処理
	PlayerDirectionUpdate();

	chargeTime_++;

	// 1秒に一回チャージSEを鳴らす
	if (chargeTime_ % 60 == 0) {
		SoundManager::GetInstance()->PlaySe(SeType::kCharge);
	}

	// 攻撃キーリリース時に弾生成
	if (GameInput::GetInstance()->IsRelease(GameAction::kAttack)) {
		// チャージ段階
		int chargeStep = GetChargeStep();

		// 弾生成
		PlayerBullet* bullet = new PlayerBullet();

		// パーティクル管理先を設定
		bullet->SetTrailParticles(bulletTrailParticles_);

		// 弾初期化
		bullet->Initialize(camera_, worldTransform_.translation_, static_cast<Direction>(direction_), chargeStep);

		// 配列更新
		bullets_.push_back(bullet);

		ammo_--;

		// 通常状態へのリクエスト
		behaviorRequest_ = Behavior::kRoot;

		switch (chargeStep) {
		case 0:
			// SEを鳴らす
			SoundManager::GetInstance()->PlaySe(SeType::kShot1);

			break;

		case 1:
			// SEを鳴らす
			SoundManager::GetInstance()->PlaySe(SeType::kShot2);

			// 下降
			if (direction_ == Direction::kUp) {
				isJumpThrough_ = true;
				isGround_ = false;
			}
			break;

		case 2:
			// SEを鳴らす
			SoundManager::GetInstance()->PlaySe(SeType::kShot3);

			// 2階分下降する処理
			// 仮置きで1段階の処理をコピペ
			if (direction_ == Direction::kUp) {
				isJumpThrough_ = true;
				isGround_ = false;
			}
			break;
		}


		// ジャンプ
		if (direction_ == Direction::kDown) {
			if (isGround_) {
				velocity_ += Vector3(0.0f, kJumpPowerTable[static_cast<size_t>(chargeStep)], 0.0f);
				isGround_ = false;
			}
		}

		chargeTime_ = 0;
	}

	// 通過ポイント更新
	JumpThroughUpdate();

	ResolveMapChipCollision(velocity_);
}

int Player::GetChargeStep() const {
	int result = 0;

	if (chargeTime_ >= kChargeStepOneTime) {
		result = 1;
	}

	if (chargeTime_ >= kChargeStepTwoTime) {
		result = 2;
	}

	return result;
}

void Player::ChangeRotationToDirection() {

	// 方向に応じた目標回転角
	float destinationRotationZ = 0.0f;

	switch (static_cast<Direction>(direction_)) {
	case Direction::kUp:
		destinationRotationZ = kPi / 2.0f;
		break;

	case Direction::kRightUp:
		destinationRotationZ = kPi / 4.0f;
		break;

	case Direction::kRight:
		destinationRotationZ = 0.0f;
		break;

	case Direction::kRightDown:
		destinationRotationZ = -kPi / 4.0f;
		break;

	case Direction::kDown:
		destinationRotationZ = -kPi / 2.0f;
		break;

	default:
		destinationRotationZ = 0.0f;
		break;
	}

	// 照準方向が変化した場合
	if (previousAimDirection_ != direction_) {

		// 現在の角度を旋回開始角度として保存
		aimStartRotationZ_ = worldTransform_.rotation_.z;

		// 旋回時間を初期化
		aimTurnTimer_ = 0.0f;

		// 現在の方向を保存
		previousAimDirection_ = direction_;
	}

	// 旋回時間を進める
	if (aimTurnTimer_ < kAimTurnTime) {
		aimTurnTimer_ += 1.0f / 60.0f;
	}

	// 補間割合を算出
	float t = std::clamp(aimTurnTimer_ / kAimTurnTime, 0.0f, 1.0f);

	// イージング
	float easedT = Ease::EaseInOut(t);

	// 目標角度まで補間
	worldTransform_.rotation_.z = std::lerp(aimStartRotationZ_, destinationRotationZ, easedT);
}

void Player::UpdateChargeShake() {

	// 描画用ワールド変換に通常の変換情報を反映
	drawWorldTransform_.translation_ = worldTransform_.translation_;
	drawWorldTransform_.rotation_ = worldTransform_.rotation_;
	drawWorldTransform_.scale_ = worldTransform_.scale_;

	// 振動幅と速度
	float shakeAmplitude = 0.0f;
	float shakeSpeed = 0.0f;

	// チャージ中のみ振動
	if (behavior_ == Behavior::kCharge) {

		// チャージ段階を取得
		int chargeStep = GetChargeStep();

		if (chargeStep == 1) {
			shakeAmplitude = kChargeStepOneShakeAmplitude;
			shakeSpeed = kChargeStepOneShakeSpeed;

		} else if (chargeStep >= 2) {
			shakeAmplitude = kChargeStepTwoShakeAmplitude;
			shakeSpeed = kChargeStepTwoShakeSpeed;
		}
	}

	// 振動する場合
	if (shakeAmplitude > 0.0f) {

		// 振動時間を進める
		chargeShakeTimer_ += 1.0f / 60.0f;

		// X方向の振動
		drawWorldTransform_.translation_.x += std::sin(chargeShakeTimer_ * shakeSpeed) * shakeAmplitude;

		// Y方向の振動
		drawWorldTransform_.translation_.y += std::sin(chargeShakeTimer_ * shakeSpeed * 1.7f) * shakeAmplitude;

	} else {

		// 振動していないときは時間をリセット
		chargeShakeTimer_ = 0.0f;
	}

	// 描画用ワールド行列を更新
	WorldTransformUpdate(drawWorldTransform_);
}

void Player::BehaviorKnockInitialize() {
	// 攻撃中の変形を戻す
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// ノックバック状態を初期化
	knockbackPhase_ = KnockbackPhase::kKnockback;
	knockbackParameter_ = 0.0f;

	// 向いている方向と逆方向へ吹っ飛ばす
	float knockbackDirection = 0.0f;

	if (lrDirection_ == LRDirection::kRight) {
		knockbackDirection = -1.0f;
	} else {
		knockbackDirection = 1.0f;
	}

	velocity_ = {knockbackDirection * kKnockbackSpeed, 0.0f, 0.0f};
}

void Player::BehaviorKnockUpdate() {
	knockbackParameter_ += 1.0f / 60.0f;

	switch (knockbackPhase_) {
	case KnockbackPhase::kKnockback:
		// 真横ノックバックなのでY方向には動かさない
		velocity_.y = 0.0f;

		// ノックバック移動
		ResolveMapChipCollision(velocity_);

		if (knockbackParameter_ >= kKnockbackTime) {
			knockbackPhase_ = KnockbackPhase::kAfter;
			knockbackParameter_ = 0.0f;
		}

		break;

	case KnockbackPhase::kAfter:
		// 横移動を減速
		velocity_.x *= (1.0f - kKnockbackAttenuation);

		// 真横ノックバックなのでY方向には動かさない
		velocity_.y = 0.0f;

		ResolveMapChipCollision(velocity_);

		if (knockbackParameter_ >= kKnockbackAfterTime) {
			velocity_ = {0.0f, 0.0f, 0.0f};
			behaviorRequest_ = Behavior::kRoot;
		}

		break;
	}
}

void Player::ResolveMapChipCollision(const Vector3& moveDistance) {
	// 衝突判定の初期化
	CollisionMapInfo collisionMapInfo{};

	// 移動量のコピー
	collisionMapInfo.moveDistance = moveDistance;

	// マップ衝突チェック
	MapChipCollision(collisionMapInfo);

	// 移動
	ApplyCollisionResult(collisionMapInfo);

	// 天井に接触している時の処理
	HandleCeilingCollision(collisionMapInfo);

	// 壁に接触している時の処理
	HandleWallCollision(collisionMapInfo);

	// 接地状態の切り替え処理
	ChangeGroundState(collisionMapInfo);
}

void Player::MapChipCollision(CollisionMapInfo& info) {
	//MapChipCollisionTop(info);
	MapChipCollisionBottom(info);
	MapChipCollisionLeft(info);
	MapChipCollisionRight(info);
}

void Player::MapChipCollisionTop(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.moveDistance.y <= 0.0f) {
		return;
	}

	// 通り抜ける
	if (isJumpThrough_) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); i++) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveDistance, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真上の判定を行う
	bool isHit = false;
	IndexSet indexSet{};
	IndexSet hitIndexSet{};

	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	
	//判定するx場所を固定
	indexSet.xIndex = 0;
	
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);

	// 判定するx場所を固定
	indexSet.xIndex = 0;

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	if (isHit) {
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLeftTop));

		// 移動前と移動後でY方向のセル番号が変わった場合だけ、天井として処理する
		if (indexSetNow.yIndex != hitIndexSet.yIndex) {
			Rect rect = mapChipField_->RectByIndex(hitIndexSet.xIndex, hitIndexSet.yIndex);

			info.moveDistance.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight / 2.0f - kBlank);

			info.isHitCeiling = true;
		}
	}
}

void Player::MapChipCollisionBottom(CollisionMapInfo& info) {
	// 下降あり？
	if (info.moveDistance.y >= 0.0f) {
		return;
	}

	// 通り抜ける
	if (isJumpThrough_) {
		if (jumpLowerLimitsPoint.isOver == true) {

			return;
		}

	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveDistance, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真下の当たり判定を行う
	bool isHit = false;
	IndexSet indexSet{};
	IndexSet hitIndexSet{};

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);

	// 判定するx場所を固定
	indexSet.xIndex = 0;

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);

	// 判定するx場所を固定
	indexSet.xIndex = 0;

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	if (isHit) {
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLeftBottom));

		// 移動前と移動後でY方向のセル番号が変わった場合だけ、地面として処理する
		if (indexSetNow.yIndex != hitIndexSet.yIndex) {
			Rect rect = mapChipField_->RectByIndex(hitIndexSet.xIndex, hitIndexSet.yIndex);

			info.moveDistance.y = std::min(0.0f, rect.top - worldTransform_.translation_.y + kHeight / 2.0f + kBlank);

			info.onGround = true;
		}
	}
}

void Player::MapChipCollisionLeft(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.moveDistance.x >= 0.0f) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveDistance, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真左の当たり判定を行う
	bool isHit = false;
	IndexSet indexSet{};
	IndexSet hitIndexSet{};

	// 左上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);

	// 隣接セルがどちらもブロックなら床や天井扱いなので、壁としては扱わない
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);

	// 隣接セルがどちらもブロックなら床や天井扱いなので、壁としては扱わない
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	if (isHit) {
		// 現在座標が属するマップチップ番号
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kLeftTop));

		// 移動前と移動後でX方向のセル番号が変わった場合だけ、壁として処理する
		if (indexSetNow.xIndex != hitIndexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			Rect rect = mapChipField_->RectByIndex(hitIndexSet.xIndex, hitIndexSet.yIndex);

			// めり込みを排除する方向に移動量を設定する
			info.moveDistance.x = std::min(0.0f, rect.right - worldTransform_.translation_.x + kWidth / 2.0f + kBlank);

			// 壁に当たったことを記録する
			info.isHitWall = true;
		}
	}
}

void Player::MapChipCollisionRight(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.moveDistance.x <= 0.0f) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew{};

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.moveDistance, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真右の当たり判定を行う
	bool isHit = false;
	IndexSet indexSet{};
	IndexSet hitIndexSet{};

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);

	// 隣接セルがどちらもブロックなら床や天井扱いなので、壁としては扱わない
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);

	// 隣接セルがどちらもブロックなら床や天井扱いなので、壁としては扱わない
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		isHit = true;
		hitIndexSet = indexSet;
	}

	if (isHit) {
		// 現在座標が属するマップチップ番号
		IndexSet indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(CornerPosition(worldTransform_.translation_, kRightTop));

		// 移動前と移動後でX方向のセル番号が変わった場合だけ、壁として処理する
		if (indexSetNow.xIndex != hitIndexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			Rect rect = mapChipField_->RectByIndex(hitIndexSet.xIndex, hitIndexSet.yIndex);

			// めり込みを排除する方向に移動量を設定する
			info.moveDistance.x = std::max(0.0f, rect.left - worldTransform_.translation_.x - kWidth / 2.0f - kBlank);

			// 壁に当たったことを記録する
			info.isHitWall = true;
		}
	}
}

void Player::ApplyCollisionResult(const CollisionMapInfo& info) { worldTransform_.translation_ += info.moveDistance; }

void Player::HandleCeilingCollision(const CollisionMapInfo& info) {
	if (info.isHitCeiling) {
		velocity_.y = 0.0f;
	}
}

// 地面に接触してる場合の処理
void Player::HandleGroundCollision(const CollisionMapInfo& info) {
	if (info.onGround) {
		// 着地状態に移行
		isGround_ = true;

		// 接地時にx速度を減衰
		velocity_.x *= (1.0f - kAttenuationLanding);

		velocity_.y = 0.0f;
	}
}

void Player::HandleWallCollision(const CollisionMapInfo& info) {
	if (info.isHitWall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

// 接地状態の切り替え処理
void Player::ChangeGroundState(const CollisionMapInfo& info) {
	if (isGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			isGround_ = false;
			return;
		}

		// 落下開始判定
		MapChipType mapChipType;
		bool isHit = false;

		// 左下点の判定
		Vector3 leftBottom = CornerPosition(worldTransform_.translation_, kLeftBottom);
		leftBottom += Vector3(0.0f, -kGroundCheckOffset, 0.0f);

		IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);

		// 横一列に床が存在する仕様のため、Xインデックスを0に固定
		indexSet.xIndex = 0;

		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

		if (mapChipType == MapChipType::kBlock) {
			isHit = true;
		}

		// 右下点の判定
		Vector3 rightBottom = CornerPosition(worldTransform_.translation_, kRightBottom);
		rightBottom += Vector3(0.0f, -kGroundCheckOffset, 0.0f);

		indexSet = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);
		mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

		if (mapChipType == MapChipType::kBlock) {
			isHit = true;
		}

		// 床がなくなったら空中状態に移行
		if (!isHit) {
			isGround_ = false;
		}

	} else {
		// 空中から地面に接地する際の処理
		HandleGroundCollision(info);
	}
}

const WorldTransform& Player::GetWorldTransform() { return worldTransform_; }

Vector3 Player::GetWorldPosition() const {
	Vector3 worldPosition{};
	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];
	return worldPosition;
}

AABB Player::GetAABB() const {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb{};
	aabb.min = {worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f};
	aabb.max = {worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f};

	return aabb;
}

void Player::OnCollision() {
	// すでに死んでいたら早期リターン
	if (isDead_) {
		return;
	}

	// ノックバック初めの時間に無敵でないと、そのまま敵にめり込んで死ぬ場合があるので、吹っ飛ばされている間は無敵
	if (IsKnockbackInvincible()) {
		return;
	}

	// 被弾後無敵中
	if (damageInvincibleTimer_ > 0.0f) {
		return;
	}

	// HPを減らす
	HP_ -= 1;

	// 被弾後無敵を開始
	damageInvincibleTimer_ = kDamageInvincibleTime;

	// ヒットフラッシュ表示開始
	damageColorTimer_ = kDamageColorTime;

	// HPが残っているなら終了
	if (HP_ > 0) {
		// ダメージSEを鳴らす
		SoundManager::GetInstance()->PlaySe(SeType::kPlayerDamage);

		return;
	}

	// 通常死亡状態にする
	deathType_ = DeathType::kNormal;
	isDead_ = true;

	// 死亡SEを鳴らす
	SoundManager::GetInstance()->PlaySe(SeType::kPlayerDeath);
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[static_cast<uint32_t>(Corner::kNumCorner)] = {
	    Vector3(kWidth / 2.0f, -kHeight / 2.0f, 0.0f),  // 右下
	    Vector3(-kWidth / 2.0f, -kHeight / 2.0f, 0.0f), // 左下
	    Vector3(kWidth / 2.0f, kHeight / 2.0f, 0.0f),   // 右上
	    Vector3(-kWidth / 2.0f, kHeight / 2.0f, 0.0f)   // 左上
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::RegisterGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	globalVariables->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "LimitRunSpeed", kLimitRunSpeed);
	globalVariables->AddItem(groupName, "AttenuationLanding", kAttenuationLanding);
	globalVariables->AddItem(groupName, "AttenuationWall", kAttenuationWall);
	globalVariables->AddItem(groupName, "TurnTime", kTurnTime);

	globalVariables->AddItem(groupName, "KnockbackTime", kKnockbackTime);
	globalVariables->AddItem(groupName, "KnockbackAfterTime", kKnockbackAfterTime);
	globalVariables->AddItem(groupName, "KnockbackSpeed", kKnockbackSpeed);
	globalVariables->AddItem(groupName, "KnockbackAttenuation", kKnockbackAttenuation);
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	kLimitRunSpeed = globalVariables->GetFloatValue(groupName, "LimitRunSpeed");
	kAttenuationLanding = globalVariables->GetFloatValue(groupName, "AttenuationLanding");
	kAttenuationWall = globalVariables->GetFloatValue(groupName, "AttenuationWall");
	kTurnTime = globalVariables->GetFloatValue(groupName, "TurnTime");

	kKnockbackTime = globalVariables->GetFloatValue(groupName, "KnockbackTime");
	kKnockbackAfterTime = globalVariables->GetFloatValue(groupName, "KnockbackAfterTime");
	kKnockbackSpeed = globalVariables->GetFloatValue(groupName, "KnockbackSpeed");
	kKnockbackAttenuation = globalVariables->GetFloatValue(groupName, "KnockbackAttenuation");
}

void Player::ClearBullets() {

	// 全ての弾を削除
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	// 配列を空にする
	bullets_.clear();
}

void Player::CorrectPositionInScreen() {
	// カメラが未設定なら何もしない
	if (!camera_) {
		return;
	}

	// カメラ位置を基準に画面端のワールド座標を計算
	const float leftLimit = camera_->translation_.x + kScreenLeftLimit + kWidth / 2.0f;
	const float rightLimit = camera_->translation_.x + kScreenRightLimit - kWidth / 2.0f;
	const float bottomLimit = camera_->translation_.y + kScreenBottomLimit + kHeight / 2.0f;
	const float topLimit = camera_->translation_.y + kScreenTopLimit - kHeight / 2.0f;

	// 画面外に出ないように座標を補正
	worldTransform_.translation_.x = std::clamp(worldTransform_.translation_.x, leftLimit, rightLimit);
	worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, bottomLimit, topLimit);
}

void Player::CheckScreenSqueezeDeath() {
	// 強制スクロール中でなければ判定しない
	if (!isScreenSqueezeDeathEnabled_) {
		return;
	}

	// すでに死亡しているなら何もしない
	if (isDead_) {
		return;
	}

	// 画面左端にいなければ挟まれていない
	if (!IsTouchingLeftScreenEdge()) {
		return;
	}

	// 右側にブロックがあれば、画面端とブロックに挟まれて死亡
	if (IsBlockOnRightSide()) {
		StartSqueezeDeath();
	}
}

bool Player::IsTouchingLeftScreenEdge() {
	if (!camera_) {
		return false;
	}

	// プレイヤー中心座標として許される左端
	const float leftLimit = camera_->translation_.x + kScreenLeftLimit + kWidth / 2.0f;

	return worldTransform_.translation_.x <= leftLimit + kScreenEdgeCheckTolerance;
}

bool Player::IsBlockOnRightSide() {
	if (!mapChipField_) {
		return false;
	}

	// 右上と右下を少しだけ右側にずらして、壁があるか調べる
	KamataEngine::Vector3 rightTop = {
	    worldTransform_.translation_.x + kWidth / 2.0f + kScreenSqueezeCheckOffset, worldTransform_.translation_.y + kHeight / 2.0f - kScreenSqueezeCheckOffset, worldTransform_.translation_.z};

	KamataEngine::Vector3 rightBottom = {
	    worldTransform_.translation_.x + kWidth / 2.0f + kScreenSqueezeCheckOffset, worldTransform_.translation_.y - kHeight / 2.0f + kScreenSqueezeCheckOffset, worldTransform_.translation_.z};

	IndexSet indexSet{};

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(rightTop);

	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		return true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);

	if (mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex) == MapChipType::kBlock) {
		return true;
	}

	return false;
}

void Player::StartSqueezeDeath() {
	deathType_ = DeathType::kSqueeze;
	isDead_ = true;
	isDeathMotionFinished_ = false;

	behaviorRequest_ = Behavior::kSqueezeDeath;

	// 通常移動の速度を一旦止める
	velocity_ = {0.0f, 0.0f, 0.0f};
}

void Player::BehaviorSqueezeDeathInitialize() {
	// 攻撃中の変形が残らないように戻す
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 演出タイマー初期化
	squeezeDeathTimer_ = 0.0f;

	// 少し上へ飛ばす
	velocity_ = {0.0f, kSqueezeDeathJumpVelocity, 0.0f};

	// 演出終了フラグを戻す
	isDeathMotionFinished_ = false;
}

void Player::BehaviorSqueezeDeathUpdate() {
	squeezeDeathTimer_ += 1.0f / 60.0f;

	// 重力で落下
	velocity_.y -= kSqueezeDeathGravity;

	// 移動
	worldTransform_.translation_ += velocity_;

	// X回転しながら吹っ飛ぶ
	worldTransform_.rotation_.x += kSqueezeDeathRotationSpeed * (1.0f / 60.0f);

	// 一定時間経過、または画面下の少し下まで落ちたら演出終了
	const float bottomLimit = camera_->translation_.y + kScreenBottomLimit - 2.0f;

	if (squeezeDeathTimer_ >= kSqueezeDeathMotionTime || worldTransform_.translation_.y < bottomLimit) {
		isDeathMotionFinished_ = true;
	}
}

bool Player::ShouldDraw() const {
	// 生きているなら描画する
	if (!isDead_) {
		return true;
	}

	// 挟まれ死亡中はプレイヤー本体を描画する
	if (deathType_ == DeathType::kSqueeze) {
		return true;
	}

	// 通常死亡では描画しない
	return false;
}

void Player::PlayerDirectionUpdate() {
	// ゲームインプットのインスタンス取得
	GameInput *input = GameInput::GetInstance();

	// 向いている方向ベクトル
	int directionVectorX = 0;
	int directionVectorY = 0;

	// 上入力
	if (input->IsPress(GameAction::kDirectionUp)) {
		directionVectorY += 1;
	}

	// 下入力
	if (input->IsPress(GameAction::kDirectionDown)) {
		directionVectorY -= 1;
	}

	// 右入力
	if (input->IsPress(GameAction::kDirectionRight)) {
		directionVectorX += 1;
	}

	//// 左入力
	//// (現状未実装のためコメントアウト)
	//if (input->IsPress(GameAction::kDirectionLeft)) {
	//	directionVectorX -= 1;
	//}

	// 方向ベクトルをDirectionに変換
	// xは10の位、yは1の位を扱うため桁変換を行う
	direction_ = kCenter + (directionVectorX * 10 + directionVectorY);

	// 未入力なら右向き
	if (direction_ == kCenter) {
		direction_ = kRight;
	}
}

void Player::UpdateDamageInvincibility() {
	const float deltaTime = 1.0f / 60.0f;

	// 無敵時間が残っていなければ何もしない
	if (damageInvincibleTimer_ <= 0.0f) {
		damageInvincibleTimer_ = 0.0f;
		return;
	}

	damageInvincibleTimer_ -= deltaTime;

	if (damageInvincibleTimer_ < 0.0f) {
		damageInvincibleTimer_ = 0.0f;
	}
}

void Player::UpdateDamageVisual() {
	const float deltaTime = 1.0f / 60.0f;

	// 被弾直後の赤表示時間を更新
	if (damageColorTimer_ > 0.0f) {
		damageColorTimer_ -= deltaTime;

		if (damageColorTimer_ < 0.0f) {
			damageColorTimer_ = 0.0f;
		}
	}

	//============================================================
	// 被弾直後は赤
	//============================================================

	if (damageColorTimer_ > 0.0f) {
		objectColorValue_ = {1.0f, 0.2f, 0.2f, 1.0f};

		objectColor_.SetColor(objectColorValue_);

		return;
	}

	// 通常色
	objectColorValue_ = {1.0f, 1.0f, 1.0f, 1.0f};

	objectColor_.SetColor(objectColorValue_);
}
