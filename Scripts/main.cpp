#include "DxLib.h"
#include "KxLib.h"
#include <string>

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "GJ4_Gamejam";

//ブロックの大きさ
const int BLOCK_RADIUS = 128;
//横列の数
const int PLAYPART_WIDTH = 7;
//1区画の縦の長さ
const int PLAYPART_HEIGHT = 100;
//UIライン
const int GAME_LINE = BLOCK_RADIUS * PLAYPART_WIDTH;

// ウィンドウ横幅
const int WIN_WIDTH = 1280;
// ウィンドウ縦幅
const int WIN_HEIGHT = 720;

struct MouseInputData {
	Vector2D position = { 0, 0 };
	Vector2D pin = { 0, 0 };
	bool click = false;
	bool preClick = false;
};

struct Button {
	rect entity;
	string text;
	string eText;
	bool toggleEnabled;
	bool toggleStatus;
	int status;
};

struct Player
{
	RigidBody rigidBody;
	bool direction;
	int stuckFrameCount;
};

enum Scene
{
	logo,
	title,
	playpart,
};

Vector2D GetMousePositionToV2D()
{
	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);
	return Vector2D{ (double)mouseX, (double)mouseY };
}

bool GetLeftClick()
{
	return GetMouseInput() & MOUSE_INPUT_LEFT;
}

void MouseInputDataUpdate(MouseInputData* mouseInputData)
{
	mouseInputData->preClick = mouseInputData->click;
	mouseInputData->click = GetLeftClick();

	mouseInputData->position = GetMousePositionToV2D();
	if (!mouseInputData->click)
	{
		mouseInputData->pin = mouseInputData->position;
	}
}

void ButtonUpdate(Button& button, MouseInputData mouseInputData, int pushSound)
{
	bool isHitMousePoint = HitRectAndPoint(button.entity, GetMousePositionToV2D());
	if (!isHitMousePoint)
	{
		button.status = 0;
		return;
	}
	else
	{
		if (mouseInputData.click) {
			if (isHitMousePoint)
			{
				button.status = 2;
				return;
			}
		}
		else
		{
			if (mouseInputData.preClick && isHitMousePoint)
			{
				PlaySoundMem(pushSound, DX_PLAYTYPE_BACK, true);
				button.status = 3;
				return;
			}
			else
			{
				button.status = 1;
			}
		}
	}
}

void DrawButton(Button button)
{
	unsigned int color[3] = { 70, 70, 70 };
	switch (button.status) {
	case 0:
		color[0] = 255;
		color[1] = 255;
		color[2] = 255;
		break;
	case 1:
		color[0] = 255;
		color[1] = 180;
		color[2] = 255;
		break;
	case 2:
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		break;
	case 3:
		color[0] = 255;
		color[1] = 0;
		color[2] = 0;
		break;
	}
	DrawBox(
		(int)(button.entity.x + button.entity.w),
		(int)(button.entity.y + button.entity.h),
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y - button.entity.h),
		GetColor(color[0], color[1], color[2]), TRUE);
	DrawLine(
		(int)(button.entity.x + button.entity.w - 1),
		(int)(button.entity.y - button.entity.h),
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y - button.entity.h),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawLine(
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y + button.entity.h - 1),
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y - button.entity.h - 1),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawTriangle(
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y + button.entity.h),
		(int)(button.entity.x + button.entity.w),
		(int)(button.entity.y - button.entity.h),
		(int)(button.entity.x + button.entity.w),
		(int)(button.entity.y + button.entity.h),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2), TRUE);
	if (button.toggleEnabled) {
		DrawBox(
			(int)(button.entity.x + button.entity.w),
			(int)(button.entity.y - button.entity.h / 2),
			(int)(button.entity.x + button.entity.w / 2),
			(int)(button.entity.y - button.entity.h), GetColor(100, 100, 100), TRUE);
		if (button.toggleStatus) {
			DrawBox(
				(int)(button.entity.x + button.entity.w / 4 * 3),
				(int)(button.entity.y - button.entity.h / 2),
				(int)(button.entity.x + button.entity.w / 2),
				(int)(button.entity.y - button.entity.h), GetColor(100, 255, 100), TRUE);
		}
		else {
			DrawBox(
				(int)(button.entity.x + button.entity.w / 4 * 3),
				(int)(button.entity.y - button.entity.h / 2),
				(int)(button.entity.x + button.entity.w),
				(int)(button.entity.y - button.entity.h), GetColor(150, 0, 0), TRUE);
		}
	}
	DrawString(
		button.entity.x - button.entity.w, button.entity.y - button.entity.h,
		button.eText.c_str(),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawString(
		button.entity.x, button.entity.y, button.text.c_str(),
		GetColor(0, 0, 0));
}

bool IsButtonClicked(vector<Button>& buttons, int buttonIndex)
{
	return buttons.size() > buttonIndex && buttons[buttonIndex].status == 3;
}

void PlayerUpdate(Player* player, std::vector<GameObject> blocks)
{
	//物理挙動
	RigidBodyUpdate(player->rigidBody, { 0,1 }, { 0.5,1 }, blocks);
	//着地していたら
	if (player->rigidBody.landing)
	{
		//しばらく前方に進めなければ反転
		if ((player->direction
			&& player->rigidBody.gameObject.beforePos.x >= player->rigidBody.gameObject.entity.x)
			|| (!player->direction
				&& player->rigidBody.gameObject.beforePos.x <= player->rigidBody.gameObject.entity.x))
		{
			player->stuckFrameCount++;
		}
		else
		{
			player->stuckFrameCount--;
		}
		player->stuckFrameCount = min(max(0, player->stuckFrameCount), 3);

		if (player->stuckFrameCount >= 3)
		{
			player->direction = !player->direction;
			player->stuckFrameCount = 0;
		}

		//前進
		float playerMoveForce = 5;
		if (!player->direction)
		{
			playerMoveForce *= -1;
		}
		player->rigidBody.movement.x += playerMoveForce;
	}
	else
	{
		player->stuckFrameCount = 0;
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {
	// ウィンドウモードに設定
	ChangeWindowMode(TRUE);

	// ウィンドウサイズを手動では変更させず、
	// かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// タイトルを変更
	SetMainWindowText(TITLE);

	// 画面サイズの最大サイズ、カラービット数を設定(モニターの解像度に合わせる)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// 画面サイズを設定(解像度との比率で設定)
	SetWindowSizeExtendRate(1.0);

	// 画面の背景色を設定する
	SetBackgroundColor(0x00, 0x00, 0x00);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// 画像などのリソースデータの変数宣言と読み込み

	//開発者ロゴ
	const int logoGraph = LoadGraph("Resources/Textures/TERAPETAGAMES_logo.png");
	//タイトル画面(タイトル画面からシューティングゲーム)とBGM
	const int titleGraph = LoadGraph("Resources/Textures/title.png");
	//クリア画面(シューティングゲームからクリア画面)とBGM
	const int clearGraph = LoadGraph("Resources/Textures/clear.png");
	//自機
	const int playerSprite = LoadGraph("Resources/Textures/napnose.png");

	//ボタンを押す音
	const int buttonPushSound = LoadSoundMem("Resources/SE/buttonPush.wav");

	// ゲームループで使う変数の宣言

	//マウス入力を扱うフィールド
	MouseInputData mouseInputData;

	//現在のシーン
	Scene currentScene = logo;
	//遷移しようとしている次のシーン
	Scene nextScene = logo;

	//シーン遷移用のタイマー
	float sceneTransitionProgress = 0;

	//ボタン
	vector<Button> buttons;

	const Vector2D mapScale = { PLAYPART_WIDTH,PLAYPART_HEIGHT };

	Player player = Player{ RigidBody{ GameObject{ rect{0,0,64,64}, playerSprite} } };
	vector<GameObject> edgeWall = {
		GameObject{rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{rect{-WIN_WIDTH / 2 + GAME_LINE,0,0,WIN_HEIGHT}},
		GameObject{rect{0,-WIN_HEIGHT / 2,WIN_WIDTH,0}},
		GameObject{rect{0,WIN_HEIGHT / 2,WIN_WIDTH,0}},
	};
	vector<vector<int>> map = { {} };


	// 最新のキーボード情報用
	char keys[256] = { 0 };

	// 1ループ(フレーム)前のキーボード情報
	char oldkeys[256] = { 0 };

	// ゲームループ
	while (true) {
		// 最新のキーボード情報だったものは1フレーム前のキーボード情報として保存
		// 最新のキーボード情報を取得
		GetHitKeyStateAll(keys);
		// 画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		// 更新処理

		//マウスを更新
		MouseInputDataUpdate(&mouseInputData);

		//ボタン配列をリセット
		buttons = {};

		//シーン遷移入力で対応するシーンへの遷移を準備
		if (!mouseInputData.click && mouseInputData.preClick)
		{
			switch (currentScene)
			{
			case logo:
				nextScene = title;
				break;
			}
		}

		bool sceneInit = false;
		if (nextScene == currentScene)
		{
			sceneTransitionProgress = 0;
		}
		else
		{
			//シーンを切り替えのカウントダウン
			sceneTransitionProgress++;
			if (sceneTransitionProgress >= 30) {
				//シーンを切り替え、初期化フラグを立てる
				currentScene = nextScene;
				sceneInit = true;
			}
		}

		//シーン初期化処理
		if (sceneInit)
		{
			switch (currentScene)
			{
			case title:
				//タイトル画面
				break;
			case playpart:
				//プレイパート

				//自機を初期座標へ
				player = Player{ RigidBody{ GameObject{ rect{-WIN_WIDTH / 2 + GAME_LINE / 2,0,128,128}, playerSprite} } };
				break;
			default:
				break;
			}
		}

		//ボタンを生成
		switch (currentScene)
		{
		case title:
			//タイトル画面

			//スタートボタン
			buttons = {
				Button{rect{WIN_WIDTH / 2, WIN_HEIGHT / 6 * 5,200,100},"スタート\n","START\n"},
			};
			break;
		case playpart:
			//プレイパート
			buttons = {
				Button{rect{70, 60,60,50},"ﾎﾟｰｽﾞ\n","PAUSE\n"},
			};
			break;
		default:
			break;
		}

		//ボタンを更新
		for (int i = 0; i < buttons.size(); i++) {
			ButtonUpdate(buttons[i], mouseInputData, buttonPushSound);
		}

		switch (currentScene)
		{
		case title:
			//タイトル画面

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = playpart;
			}
			break;
		case playpart:
			//プレイパート

			//自機を更新
			PlayerUpdate(&player, edgeWall);

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		default:
			break;
		}

		// 描画処理
		switch (currentScene)
		{
		case title:
			//タイトル画面
			DrawGraph(0, 0, titleGraph, true);
			break;
		case playpart:
			//プレイパート

			//自機を描画
			RenderObject(player.rigidBody.gameObject, Vector2D{ -WIN_WIDTH / 2,WIN_HEIGHT / 2 });
			//このラインからはUIゾーンなのでいっそここにボックスUIおいてもいいや
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);
			break;
		case logo:
			//ロゴ画面
			DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 2, 0, logoGraph, 0);
			break;
		}

		//全てのボタンを描画
		for (int i = 0; i < buttons.size(); i++) {
			DrawButton(buttons[i]);
		}

		//---------  ここまでにプログラムを記述  ---------//
		// (ダブルバッファ)裏面
		ScreenFlip();

		// 20ミリ秒待機(疑似60FPS)
		WaitTimer(20);

		// Windowsシステムからくる情報を処理する
		if (ProcessMessage() == -1) {
			break;
		}

		// ESCキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}
