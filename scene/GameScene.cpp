#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>
using namespace DirectX;

GameScene::GameScene() {}

GameScene::~GameScene() {
	// delete sprite_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	textureHandle_ = TextureManager::Load("mario.jpg"); //ファイル名を指定してテクスチャを読み込む
	sprite_ = Sprite::Create(textureHandle_, {100, 0}); //スプライトの生成
	model_ = Model::Create();                           // 3Dモデルの生成
	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};
		worldTransform_[i].Initialize(); //ワールドトランスフォームの初期化
	}

	viewProjection_.up = {cosf(XM_PI / 4.0f), sin(XM_PI / 4.0f), 0.0f};
	viewProjection_.target = {10, 0, 0};
	viewProjection_.eye = {0, 0, -10};

	////カメラ垂直方向視野角を設定
	// viewProjection_.fovAngleY = XMConvertToRadians(45.0f);
	////アスペクト比を設定
	// viewProjection_.aspectRatio = 1.0f;
	//////ニアクリップ距離を設定
	// viewProjection_.nearZ = 52.0f;
	//////ファークリップ距離を設定
	// viewProjection_ .farZ = 53.0f;

	viewProjection_.Initialize(); //ビュープロジェクションの初期化
}

void GameScene::Update() {
	//値を含んだ文字列
	/*std::string translationDebug = std::string("translation:(") +
	                       std::to_string(worldTransform_.translation_.x) + std::string(".")+
	                       std::to_string(worldTransform_.translation_.y) + std::string(".")+
	                       std::to_string(worldTransform_.translation_.z) + std::string(")");
	debugText_->Print(translationDebug, 50, 50, 1.0f);


	 std::string rotationDebug =
	  std::string("rotation:(") + std::to_string(worldTransform_.rotation_.x) +
	  std::string(".") + std::to_string(worldTransform_.rotation_.y) + std::string(".") +
	                            std::to_string(worldTransform_.rotation_.z) + std::string(")");

	debugText_->Print(rotationDebug, 50, 70, 1.0f);

	std::string scaleDebug = std::string("scale:(") +
	                            std::to_string(worldTransform_.scale_.x) + std::string(".") +
	                            std::to_string(worldTransform_.scale_.y) + std::string(".") +
	                            std::to_string(worldTransform_.scale_.z) + std::string(")");

	debugText_->Print(scaleDebug, 50, 90, 1.0f);*/

	//連続回転
	XMFLOAT3 move{0, 0, 0};

	const float kEyeSpeed = 0.2f;

	if (input_->PushKey(DIK_W)) {
		move = {0, 0, kEyeSpeed};
	} else if (input_->PushKey(DIK_S)) {
		move = {0, 0, -kEyeSpeed};
	}

	viewProjection_.eye.x += move.x;
	viewProjection_.eye.y += move.y;
	viewProjection_.eye.z += move.z;

	viewProjection_.UpdateMatrix();

	const float kTargetSpeed = 0.2f;

	if (input_->PushKey(DIK_LEFT)) {
		move = {-kTargetSpeed, 0, 0};
	} else if (input_->PushKey(DIK_RIGHT)) {
		move = {kTargetSpeed, 0, 0};
	}

	viewProjection_.target.x += move.x;
	viewProjection_.target.y += move.y;
	// viewProjection_.target.z += move.z;

	viewProjection_.UpdateMatrix();

	const float kUpRotSpeed = 0.05f;

	if (input_->PushKey(DIK_SPACE)) {
		viewAngle += kUpRotSpeed;

		viewAngle = fmodf(viewAngle, XM_2PI);
	}

	viewProjection_.up = {cosf(viewAngle), sinf(viewAngle), 0.0f};
	viewProjection_.UpdateMatrix();

	// FoV変更処理
	//上キーで視野角が広がる
	// if (input_->PushKey(DIK_W)) {
	//	viewProjection_.fovAngleY += 0.01f;
	//	viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	//	//下キーでしあy角が狭まる
	// } else if (input_->PushKey(DIK_S)) {
	//	viewProjection_.fovAngleY-= 0.01f;
	//	viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);

	//}

	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
	  "target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
	  viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
	  "up:(% f, % f,% f) ", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	////デバッグ用表示
	// debugText_->SetPos(50, 110);
	// debugText_->Printf("fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));
	////上下キーでニアクリップ距離を増減
	// if (input_->PushKey(DIK_UP)) {
	//	viewProjection_.nearZ += 0.1f;
	//
	//	//下キーで視野角が狭まる
	// } else if (input_->PushKey(DIK_DOWN)) {
	//	viewProjection_.nearZ -= 0.1f;
	//
	// }

	////行列の再計算
	// viewProjection_.UpdateMatrix();

	////デバッグ用表示
	// debugText_->SetPos(50, 130);
	// debugText_->Printf("nearZ:%f", XMConvertToDegrees(viewProjection_.nearZ));
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 3Dモデルの描画
	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
