#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "GJ4_Gamejam";

//ブロックの大きさ
const int BLOCK_RADIUS = 64;
//横列の数
const int PLAYPART_WIDTH = 14;
//1区画の縦の長さ
const int PLAYPART_HEIGHT = 100;
//UIライン
const int GAME_LINE = BLOCK_RADIUS * PLAYPART_WIDTH;

// ウィンドウ横幅
const int WIN_WIDTH = 1280;
// ウィンドウ縦幅
const int WIN_HEIGHT = 720;

//フォントのサイズ
const int FONT_SIZE = 24;

enum Scene
{
	logo,
	title,
	playpart,
	credit,
	howtoplay,
};

enum BlockType
{
	weakblock,
	sandblock,
	frameblock,
	untappableblock,
	lethalblock,
};

struct MouseInputData {
	Vector2D position = { 0, 0 };
	Vector2D pin = { 0, 0 };
	bool click = false;
	bool preClick = false;
};

struct Button {
	Rect entity;
	string text;
	string eText;
	bool toggleEnabled;
	bool toggleStatus;
	int status;
};

struct Block
{
	RigidBody rigidBody;
	BlockType blockType;
	bool breaked = false;
};

struct LiveEntity
{
	RigidBody rigidBody;
	bool direction;
	int stuckFrameCount;
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
	bool isHitMousePoint = HitRectAndPoint(button.entity, mouseInputData.position);
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

void LiveEntityUpdate(LiveEntity* liveEntity, std::vector<GameObject> blocks)
{
	//物理挙動
	RigidBodyUpdate(liveEntity->rigidBody, { 0,1 }, { 0.5,1 }, blocks);
	//着地していたら
	if (liveEntity->rigidBody.landing)
	{
		//しばらく前方に進めなければ反転
		if ((liveEntity->direction
			&& liveEntity->rigidBody.gameObject.beforePos.x >= liveEntity->rigidBody.gameObject.entity.x)
			|| (!liveEntity->direction
				&& liveEntity->rigidBody.gameObject.beforePos.x <= liveEntity->rigidBody.gameObject.entity.x))
		{
			liveEntity->stuckFrameCount++;
		}
		else
		{
			liveEntity->stuckFrameCount--;
		}
		liveEntity->stuckFrameCount = min(max(0, liveEntity->stuckFrameCount), 3);

		if (liveEntity->stuckFrameCount >= 3)
		{
			liveEntity->direction = !liveEntity->direction;
			liveEntity->stuckFrameCount = 0;
		}

		//前進
		float playerMoveForce = 5;
		if (!liveEntity->direction)
		{
			playerMoveForce *= -1;
		}
		liveEntity->rigidBody.movement.x += playerMoveForce;
	}
	else
	{
		liveEntity->stuckFrameCount = 0;
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
	SetBackgroundColor(0xff, 0xee, 0xaa);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// フォントのサイズを設定する
	SetFontSize(FONT_SIZE);

	// 画像などのリソースデータの変数宣言と読み込み

	//開発者ロゴ
	const int logoGraph = LoadGraph("Resources/Textures/TERAPETAGAMES_logo.png");
	//タイトル画面(タイトル画面からシューティングゲーム)とBGM
	const int titleGraph = LoadGraph("Resources/Textures/title.png");
	const int operationGraph = LoadGraph("Resources/Textures/sousa.png");
	//クリア画面(シューティングゲームからクリア画面)とBGM
	const int clearGraph = LoadGraph("Resources/Textures/clear.png");
	//自機
	const int playerSprite = LoadGraph("Resources/Textures/frameBlock.png");
	//ブロック各種
	const int blocksSprite[] = {
		LoadGraph("Resources/Textures/weakBlock.png"),
		LoadGraph("Resources/Textures/sandBlock.png"),
		LoadGraph("Resources/Textures/frameBlock.png"),
		LoadGraph("Resources/Textures/unTappableBlock.png"),
		LoadGraph("Resources/Textures/lethalBlock.png"),
	};

	//ビットマップフォント用の数字画像とタイマー
	const int zeroGraph = LoadGraph("Resources/Textures/timer0.png");
	const int oneGraph = LoadGraph("Resources/Textures/timer1.png");
	const int twoGraph = LoadGraph("Resources/Textures/timer2.png");
	const int threeGraph = LoadGraph("Resources/Textures/timer3.png");
	const int fourGraph = LoadGraph("Resources/Textures/timer4.png");
	const int fiveGraph = LoadGraph("Resources/Textures/timer5.png");
	const int sixGraph = LoadGraph("Resources/Textures/timer6.png");
	const int sevenGraph = LoadGraph("Resources/Textures/timer7.png");
	const int eightGraph = LoadGraph("Resources/Textures/timer8.png");
	const int nineGraph = LoadGraph("Resources/Textures/timer9.png");
	int timer = 0;
	//1の位のタイマー
	int timerT1 = 0;
	//10の位のタイマー
	int timer2 = 0;
	//100の位のタイマー
	int timer3 = 0;


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
	//Pause中のフラグ
	bool isPause = false;
	//ボタン
	vector<Button> buttons;

	//カメラ座標
	Vector2D camPosition = Vector2D{ 0,0 };
	const Vector2D camPosOffset = Vector2D{ -WIN_WIDTH / 2,-WIN_HEIGHT / 2 };


	//自機
	LiveEntity player = LiveEntity{ RigidBody{ GameObject{ Rect{0,0,64,64}, playerSprite} } };
	//自機がフィールド外に出ないための壁
	vector<GameObject> edgeWall = {
		GameObject{Rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{Rect{-WIN_WIDTH / 2 + GAME_LINE,0,0,WIN_HEIGHT}},
		GameObject{Rect{0,-WIN_HEIGHT / 2,WIN_WIDTH,0}},
		GameObject{Rect{0,WIN_HEIGHT / 2,WIN_WIDTH,0}},
	};
	//ブロック
	vector<Block> blocks = {};


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

				//ポーズ解除
				isPause = false;
				//自機を初期座標へ
				player = LiveEntity{ RigidBody{ GameObject{ Rect{-WIN_WIDTH / 2 + GAME_LINE / 2,-WIN_HEIGHT / 2 + 32,50,64}, playerSprite} } };
				//ブロックを初期化、生成
				blocks = {};
				for (int i = 0; i < PLAYPART_HEIGHT; i++)
				{
					for (int j = 0; j < PLAYPART_WIDTH; j++)
					{
						BlockType currentBlockType = (BlockType)(rand() % 5);
						if (i == 0)
						{
							currentBlockType = weakblock;
						}

						blocks.push_back(Block{ RigidBody{GameObject{
							Rect{(float)-WIN_WIDTH / 2 + BLOCK_RADIUS / 2 + BLOCK_RADIUS * j,(float)BLOCK_RADIUS * i,BLOCK_RADIUS,BLOCK_RADIUS}
						}},currentBlockType });
					}
				}
				break;
			}
		}

		//ボタンを生成
		switch (currentScene)
		{
		case title:
			//タイトル画面

			//スタートボタンとクレジットボタン
			buttons = {
				Button{Rect{WIN_WIDTH / 2, WIN_HEIGHT / 4 * 3,200,100},"スタート","START"},
				Button{Rect{140, 60,130,50},"クレジット","CREDITS"},
				Button{Rect{WIN_WIDTH - 140, 60,130,50},"遊び方","HOW TO PLAY"} ,
			};
			break;
		case playpart:
			//プレイパート

			if (isPause)
			{
				//ポーズメニューボタン
				buttons = {
					Button{Rect{370, 160,60,50},"つづける","RESUME"},
					Button{Rect{370, 460,60,50},"やめる","QUIT"},
				};
			}
			else
			{
				//ポーズボタン
				buttons = {
					Button{Rect{70, 60,60,50},"ﾎﾟｰｽﾞ","PAUSE"},
				};
			}
			break;
		case credit:
			//クレジット画面

			//タイトルに戻るボタン
			buttons = {
				Button{Rect{140, 60,130,50},"もどる","RETURN"},
			};
			break;
		case howtoplay:
			//遊び方説明画面

			//タイトルに戻るボタン
			buttons = {
				Button{Rect{140, 60,130,50},"もどる","RETURN"},
			};
			break;
		}

		//ボタンを更新（ちょっとだけ縦に揺らす）
		for (int i = 0; i < buttons.size(); i++) {
			buttons[i].entity.y += sin(clock() / PI / 300 - i / 2.0) * 5;
			ButtonUpdate(buttons[i], mouseInputData, buttonPushSound);
		}

		switch (currentScene)
		{
		case logo:
			//ロゴ画面

			//左クリックでタイトルへ
			if (!mouseInputData.click && mouseInputData.preClick)
			{
				nextScene = title;
			}
			break;
		case title:
			//タイトル画面

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = playpart;
			}
			if (IsButtonClicked(buttons, 1))
			{
				nextScene = credit;
			}
			if (IsButtonClicked(buttons, 2))
			{
				nextScene = howtoplay;
			}
			break;
		case playpart:
			//プレイパート
			if (!isPause)
			{
				timer++;
				if (timer >= 58) {
					timerT1++;
					timer = 0;
				}
				if (timerT1 >= 10) {
					timerT1 = 0;
				}


				//全てのブロックを更新
				for (int i = 0; i < blocks.size(); i++)
				{
					blocks[i].rigidBody.gameObject.graphNum = blocksSprite[blocks[i].blockType];
					//クリックされたら消す準備（破壊可能なブロックのみ）
					if (blocks[i].blockType != untappableblock && blocks[i].blockType != lethalblock
						&& HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.position + (camPosition + camPosOffset))
						&& HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.pin + (camPosition + camPosOffset))
						&& !mouseInputData.click && mouseInputData.preClick)
					{
						blocks[i].breaked = true;
					}
				}
				//消す準備が出来たブロックを全部消す
				for (int i = 0; i < blocks.size(); i++)
				{
					if (blocks[i].breaked)
					{
						blocks.erase(blocks.begin() + i);
						i--;
					}
				}

				//四隅の壁とブロック（フレームブロック以外）を壁とする
				vector<GameObject> liveEntityWalls = edgeWall;
				for (int i = 0; i < blocks.size(); i++)
				{
					if (blocks[i].blockType != frameblock)
					{
						liveEntityWalls.push_back(blocks[i].rigidBody.gameObject);
					}
				}
				//自機を更新
				LiveEntityUpdate(&player, liveEntityWalls);

				//カメラ追従
				camPosition = Vector2D{ 0,player.rigidBody.gameObject.entity.y };

				//ボタンを押した時の処理
				if (IsButtonClicked(buttons, 0))
				{
					//ポーズする
					isPause = true;
				}
			}
			else
			{
				//ポーズ中

				//続ける
				if (IsButtonClicked(buttons, 0))
				{
					//ポーズ解除
					isPause = false;
				}
				//終わる
				if (IsButtonClicked(buttons, 1))
				{
					//タイトルへ
					nextScene = title;
				}
			}


			break;
		case credit:
			//クレジット画面

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		case howtoplay:
			//遊び方説明画面

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		}

		// 描画処理
		switch (currentScene)
		{
		case logo:
			//ロゴ画面
			DrawString(
				WIN_WIDTH / 7 * 3, WIN_HEIGHT / 4,
				"Developed by",
				GetColor(0, 0, 0));
			DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 2, 0, logoGraph, TRUE);
			DrawString(
				WIN_WIDTH / 7 * 3, WIN_HEIGHT / 5 * 3,
				"TEAM GJ4",
				GetColor(0, 0, 0));
			if (clock() % 1500 < 1000)
			{
				DrawString(
					WIN_WIDTH / 3, WIN_HEIGHT - FONT_SIZE * 4,
					"画面を左クリックしてください",
					GetColor(0, 0, 0));
			}
			break;
		case title:
			//タイトル画面

			//タイトルロゴ
			DrawGraph(0, 0, titleGraph, true);
			DrawGraph(0, 0, operationGraph, true);
			//権利表示
			DrawString(
				WIN_WIDTH / 3, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "2024 TERAPETA GAMES / TEAM GJ4",
				GetColor(0, 0, 0));
			break;
		case playpart:
			//プレイパート

			//全てのブロックを描画
			for (int i = 0; i < blocks.size(); i++) {
				RenderObject(blocks[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//自機を描画
			RenderObject(player.rigidBody.gameObject, camPosition + camPosOffset);
			//このラインからはUIゾーンなのでいっそここにボックスUIおいてもいいや
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);

			//ビットマップフォントタイマー
			if (timerT1 == 0) { DrawGraph(950, 60, zeroGraph, true); }
			else if (timerT1 == 1) { DrawGraph(950, 60, oneGraph, true); }
			else if (timerT1 == 2) { DrawGraph(950, 60, twoGraph, true); }
			else if (timerT1 == 3) { DrawGraph(950, 60, threeGraph, true); }
			else if (timerT1 == 4) { DrawGraph(950, 60, fourGraph, true); }
			else if (timerT1 == 5) { DrawGraph(950, 60, fiveGraph, true); }
			else if (timerT1 == 6) { DrawGraph(950, 60, sixGraph, true); }
			else if (timerT1 == 7) { DrawGraph(950, 60, sevenGraph, true); }
			else if (timerT1 == 8) { DrawGraph(950, 60, eightGraph, true); }
			else if (timerT1 == 9) { DrawGraph(950, 60, nineGraph, true); }

			DrawFormatString(800, 300, GetColor(122, 112, 122), "%d", timer);

			break;
		case credit:
			//クレジット画面
			DrawString(
				WIN_WIDTH / 4, 0,
				"\n\nプログラマー\n　鰯ユウ\n　神無月\n\nチーフプログラマー\n　てらぺた\n\nアート\n　てらぺた\n\nエフェクト\n　てらぺた\n\nサウンドエフェクト\n　てらぺた\n\nコンポーサー\n　てらぺた",
				GetColor(0, 0, 0));
			DrawString(
				WIN_WIDTH / 2, 0,
				"\n\nビジュアルアドバイザー\n　てらぺた\n\nエグゼクティブプロデューサー\n　てらぺた\n\nディレクター\n　てらぺた\n\nかいはつ\n　てらぺたゲームズ\n　チームGJ4\n\n\nTERAPETA GAMES / TEAM GJ4\nAll Rights Reserved.",
				GetColor(0, 0, 0));
			break;
		case howtoplay:
			DrawGraph(0, 0, operationGraph, true);
			break;
		}

		//全てのボタンを描画
		for (int i = 0; i < buttons.size(); i++)
		{
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
