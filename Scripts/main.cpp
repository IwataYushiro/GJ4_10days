#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>
#include "gameUi.h"
#include <random>

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "4005_全手動地層割り機";

//ブロックの大きさ
const int BLOCK_DIAMETER = 64;
//横列の数
const int PLAYPART_WIDTH = 14;
//1区画の縦の長さ
const int PLAYPART_HEIGHT = 25;
//UIライン
const int GAME_LINE = BLOCK_DIAMETER * PLAYPART_WIDTH;

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
	unsigned int landingBlockIndex = 0;
	float latestLandingHeight = 0;
	string status = "";
};

struct LiveEntity
{
	RigidBody rigidBody;
	int* sprites;
	float spriteIndex = 0;
	int insideBlockCount;
	bool isLive = true;
	bool direction = true;
	int stuckFrameCount = 0;
	float despawnAnimProgress = 1;
	bool isDespawned = false;
};

struct Particle {
	RigidBody rigidBody;
	float drag = 1;
	float torque = 0;
	int maxLifeTime = 20;
	int lifetime = 20;
};

Vector2D GetMousePositionToV2D()
{
	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);
	return Vector2D{ (float)mouseX, (float)mouseY };
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
		(int)(button.entity.position.x + button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y),
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y),
		GetColor(color[0], color[1], color[2]), TRUE);
	DrawLine(
		(int)(button.entity.position.x + button.entity.scale.x - 1),
		(int)(button.entity.position.y - button.entity.scale.y),
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawLine(
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y - 1),
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y - 1),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawTriangle(
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y),
		(int)(button.entity.position.x + button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y),
		(int)(button.entity.position.x + button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2), TRUE);
	if (button.toggleEnabled) {
		DrawBox(
			(int)(button.entity.position.x + button.entity.scale.x),
			(int)(button.entity.position.y - button.entity.scale.y / 2),
			(int)(button.entity.position.x + button.entity.scale.x / 2),
			(int)(button.entity.position.y - button.entity.scale.y), GetColor(100, 100, 100), TRUE);
		if (button.toggleStatus) {
			DrawBox(
				(int)(button.entity.position.x + button.entity.scale.x / 4 * 3),
				(int)(button.entity.position.y - button.entity.scale.y / 2),
				(int)(button.entity.position.x + button.entity.scale.x / 2),
				(int)(button.entity.position.y - button.entity.scale.y), GetColor(100, 255, 100), TRUE);
		}
		else {
			DrawBox(
				(int)(button.entity.position.x + button.entity.scale.x / 4 * 3),
				(int)(button.entity.position.y - button.entity.scale.y / 2),
				(int)(button.entity.position.x + button.entity.scale.x),
				(int)(button.entity.position.y - button.entity.scale.y), GetColor(150, 0, 0), TRUE);
		}
	}
	DrawString(
		button.entity.position.x - button.entity.scale.x, button.entity.position.y - button.entity.scale.y,
		button.eText.c_str(),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawString(
		button.entity.position.x, button.entity.position.y, button.text.c_str(),
		GetColor(0, 0, 0));
}

bool IsButtonClicked(vector<Button>& buttons, int buttonIndex)
{
	return buttons.size() > buttonIndex && buttons[buttonIndex].status == 3;
}

void LiveEntityUpdate(LiveEntity* liveEntity, std::vector<GameObject> blocks,
	int damageSound, int despawnSound, int* shardGraph, std::vector<Particle>& particles)
{
	//物理挙動
	RigidBodyUpdate(liveEntity->rigidBody, { 0,1 }, { 0.5,1 }, blocks);

	//落下速度制限（ブロックと同じにする）
	liveEntity->rigidBody.movement.y = min(liveEntity->rigidBody.movement.y, BLOCK_DIAMETER / 10);

	liveEntity->rigidBody.gameObject.graphLocalPos =
	{ 0,-32 + liveEntity->rigidBody.gameObject.entity.scale.x };

	//生きていたら
	if (liveEntity->isLive)
	{
		liveEntity->despawnAnimProgress = 1;

		//ブロックの中に押し込まれているか判定
		bool isInsideBlock = false;
		for (int i = 0; i < blocks.size(); i++)
		{
			if (HitRectAndPoint(blocks[i].entity,
				Vector2D{ liveEntity->rigidBody.gameObject.entity.position.x,
				liveEntity->rigidBody.gameObject.entity.position.y }))
			{
				isInsideBlock = true;
				break;
			}
		}
		//もしブロックの中に押し込まれた状態が6フレーム続いたら死ぬ
		if (isInsideBlock)
		{
			liveEntity->insideBlockCount++;
			if (liveEntity->insideBlockCount >= 6)
			{
				liveEntity->isLive = false;
			}
		}
		else
		{
			liveEntity->insideBlockCount = 0;
		}

		//着地していたら
		if (liveEntity->rigidBody.landing)
		{
			//しばらく前方に進めなければ反転
			if ((liveEntity->direction
				&& liveEntity->rigidBody.gameObject.beforePos.x >= liveEntity->rigidBody.gameObject.entity.position.x)
				|| (!liveEntity->direction
					&& liveEntity->rigidBody.gameObject.beforePos.x <= liveEntity->rigidBody.gameObject.entity.position.x))
			{
				liveEntity->stuckFrameCount++;
			}
			else
			{
				liveEntity->stuckFrameCount--;
			}
			liveEntity->stuckFrameCount = min(max(0, liveEntity->stuckFrameCount), 6);

			if (liveEntity->stuckFrameCount >= 6)
			{
				liveEntity->direction = !liveEntity->direction;
				liveEntity->rigidBody.gameObject.dir = !liveEntity->rigidBody.gameObject.dir;
				liveEntity->stuckFrameCount = 0;
			}

			//前進
			float playerMoveForce = 1;
			if (!liveEntity->direction)
			{
				playerMoveForce *= -1;
			}
			liveEntity->rigidBody.movement.x += playerMoveForce;

			liveEntity->spriteIndex = fmodf(liveEntity->spriteIndex + 0.5f, 6);
		}
		else
		{
			//空中にいる時
			liveEntity->stuckFrameCount = 0;
			liveEntity->spriteIndex = fmodf(liveEntity->spriteIndex + 1.4f, 6);
		}
	}
	else
	{
		//死んだとき

		liveEntity->spriteIndex = 6;

		liveEntity->rigidBody.gameObject.graphLocalPos.x =
			sinf(clock() / 10) * max(liveEntity->despawnAnimProgress - 0.75f, 0) * 20;

		float prevDespawnAnimProgress = liveEntity->despawnAnimProgress;
		liveEntity->despawnAnimProgress -= 0.015f;

		if (prevDespawnAnimProgress == 1)
		{
			//ダメージ音を鳴らす
			PlaySoundMem(damageSound, DX_PLAYTYPE_BACK, true);
		}

		if (prevDespawnAnimProgress > 0.5f && liveEntity->despawnAnimProgress <= 0.5f)
		{
			//破片を生成
			float rotRand = rand();
			for (int j = 0; j < 9; j++)
			{
				float angleRad = PI * 2 / 9 * j + rotRand;
				int spriteIndex = rand() % 2;
				if (j % 2 == 0)
				{
					spriteIndex += 2;
				}
				particles.push_back(Particle{ RigidBody{GameObject{
					liveEntity->rigidBody.gameObject.entity,shardGraph[spriteIndex]},
					Vector2D{sinf(angleRad),cosf(angleRad)} *15},
					0.5f + (rand() % 20) * 0.01f,(float)(rand() % 7 - 3) * 0.1f });
			}

			PlaySoundMem(despawnSound, DX_PLAYTYPE_BACK, true);
		}
	}
	liveEntity->rigidBody.gameObject.graphNum = liveEntity->sprites[(int)liveEntity->spriteIndex];

	liveEntity->despawnAnimProgress = min(max(0, liveEntity->despawnAnimProgress), 1);
	liveEntity->isDespawned = !liveEntity->isLive && liveEntity->despawnAnimProgress <= 0;
}

void GenerateLevel(std::vector<Block>& blocks, int stageLevel)
{
	//ブロック生成用のランダム
	//ランダム生成(int)
	std::random_device seedBlock;
	std::mt19937_64 engineBlock(seedBlock());
	std::uniform_real_distribution<> distTappableBlock(0, 3);
	std::uniform_real_distribution<> distUnTappableBlock(3, 5);
	std::uniform_real_distribution<> distRandomGenerate(0, 5);
	std::uniform_real_distribution<> distRandomCount(0, 5);

	//ブロックを初期化、生成
	int tappableBlockCount = 0;
	int unTappableBlockCount = 0;
	int tappableBlockCount2 = 0;
	int randomGenerateCount = 0;
	blocks = {};
	for (int i = -1; i < PLAYPART_HEIGHT; i++)
	{
		for (int j = 0; j < PLAYPART_WIDTH; j++)
		{
			if (tappableBlockCount <= 0 && unTappableBlockCount <= 0 && tappableBlockCount2 <= 0  && randomGenerateCount <= 0)
			{
				tappableBlockCount = static_cast<int>(distRandomCount(engineBlock)) + PLAYPART_WIDTH / 3;
				unTappableBlockCount = static_cast<int>(distRandomCount(engineBlock)) + min(max(0, (stageLevel - 1) * 4), PLAYPART_WIDTH * 3);
				tappableBlockCount2 = static_cast<int>(distRandomCount(engineBlock)) + PLAYPART_WIDTH / 3;
				randomGenerateCount = static_cast<int>(distRandomCount(engineBlock)) + min(max(0, (stageLevel - 1) * 8), PLAYPART_WIDTH * 3);
			}

			BlockType currentBlockType = weakblock;
			if (i == PLAYPART_HEIGHT - 1)
			{
				currentBlockType = static_cast<BlockType>(distTappableBlock(engineBlock));
			}
			else if (i == -1)
			{
				currentBlockType = frameblock;
			}
			else if(i > 0)
			{
				if (tappableBlockCount > 0)
				{
					currentBlockType = static_cast<BlockType>(distTappableBlock(engineBlock));
					tappableBlockCount--;
				}
				else if (unTappableBlockCount > 0)
				{
					currentBlockType = static_cast<BlockType>(distUnTappableBlock(engineBlock));
					unTappableBlockCount--;
				}
				else if (tappableBlockCount2 > 0)
				{
					currentBlockType = static_cast<BlockType>(distTappableBlock(engineBlock));
					tappableBlockCount2--;
				}
				else
				{
					currentBlockType = static_cast<BlockType>(distRandomGenerate(engineBlock));
					randomGenerateCount--;
				}
			}

			blocks.push_back(Block{ RigidBody{GameObject{
				Rect{(float)-WIN_WIDTH / 2 + BLOCK_DIAMETER / 2 + BLOCK_DIAMETER * j,(float)BLOCK_DIAMETER * i,
				BLOCK_DIAMETER / 2,BLOCK_DIAMETER / 2}
			}},currentBlockType });
			blocks.back().latestLandingHeight = blocks.back().rigidBody.gameObject.entity.position.y;
		}
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
	SetBackgroundColor(0xff, 0xd8, 0x86);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// フォントのサイズを設定する
	SetFontSize(FONT_SIZE);

	//クラス分けヘッダー読み込み
	gameUi* gameui_ = nullptr;
	gameui_ = new gameUi();
	gameui_->Initialize();

	// 画像などのリソースデータの変数宣言と読み込み

	//開発者ロゴ
	const int logoGraph = LoadGraph("Resources/Textures/ALHATERAPETAGAMES_logo.png");
	//タイトル画面背景
	const int bgGraph = LoadGraph("Resources/Textures/backGround.png");
	//オプション画面背景
	const int optionBGGraph = LoadGraph("Resources/Textures/optionBackGround.png");
	//タイトル画面
	const int titleGraph = LoadGraph("Resources/Textures/title.png");
	//チュートリアル
	const int tutorialGraph0 = LoadGraph("Resources/Textures/howToPlay0.png");
	const int tutorialGraph1 = LoadGraph("Resources/Textures/howToPlay1.png");
	//自機
	int playerSprites[7];
	LoadDivGraph("Resources/Textures/miniminiKX.png", 7,
		4, 4,
		64, 64, playerSprites);
	//ブロック各種
	const int blocksSprite[] = {
		LoadGraph("Resources/Textures/weakBlock.png"),
		LoadGraph("Resources/Textures/sandBlock.png"),
		LoadGraph("Resources/Textures/frameBlock.png"),
		LoadGraph("Resources/Textures/unTappableBlock.png"),
		LoadGraph("Resources/Textures/lethalBlock.png"),
	};
	//ボロブロックの破片
	int weakBlockShards[8];
	LoadDivGraph("Resources/Textures/weakBlockShard.png", 8,
		4, 2,
		32, 32, weakBlockShards);
	//砂ブロックの破片
	int sandBlockShards[8];
	LoadDivGraph("Resources/Textures/sandBlockShard.png", 8,
		4, 2,
		32, 32, sandBlockShards);
	//フレームブロックの破片
	int frameBlockShards[8];
	LoadDivGraph("Resources/Textures/frameBlockShard.png", 8,
		4, 2,
		32, 32, frameBlockShards);
	//防壁ブロックの破片
	int unTappableBlockShards[8];
	LoadDivGraph("Resources/Textures/unTappableBlockShard.png", 8,
		4, 2,
		32, 32, unTappableBlockShards);
	//リーサルブロックの破片
	int lethalBlockShards[8];
	LoadDivGraph("Resources/Textures/lethalBlockShard.png", 8,
		4, 2,
		32, 32, lethalBlockShards);
	//自機の破片
	int despawnShards[4];
	LoadDivGraph("Resources/Textures/miniminiKXShard.png", 4,
		2, 2,
		32, 32, despawnShards);
	//プレイパート背景
	const int playPartBGGraph = LoadGraph("Resources/Textures/playPartBackGround.png");

	//ボタンを押す音
	const int buttonPushSound = LoadSoundMem("Resources/SE/buttonPush.wav");
	//ブロックを壊す音
	const int blockBreakSound = LoadSoundMem("Resources/SE/breakBlock.wav");
	//壊せないブロックを押す音
	const int reflectSound = LoadSoundMem("Resources/SE/reflect.wav");
	//ダメージ音
	const int damageSound = LoadSoundMem("Resources/SE/damage.wav");
	//死んだときの音
	const int despawnSound = LoadSoundMem("Resources/SE/despawn.wav");

	//このゲームで流す全てのBGM（曲を流したくないときはインデックス0番を指定）
	const int audioClip[] = {
		0,
		LoadSoundMem("Resources/BGM/title.mp3"),
		LoadSoundMem("Resources/BGM/howToPlay.mp3"),
		LoadSoundMem("Resources/BGM/option.mp3"),
		LoadSoundMem("Resources/BGM/game.mp3"), };

	// ゲームループで使う変数の宣言

	//マウス入力を扱うフィールド
	MouseInputData mouseInputData;

	//現在のシーン
	Scene currentScene = logo;
	//遷移しようとしている次のシーン
	Scene nextScene = logo;
	//シーン遷移用のタイマー
	float sceneTransitionProgress = 0;
	//シーンの初期化が必要ならこれをtrueに
	bool sceneInit = false;
	//フェードインアウト(イン、アウト)
	int fadeInOutCount = 0;
	//今流す曲のインデックス
	int bgmNum = 0;
	//BGM、SEのボリューム
	float bgmVolume = 1;
	float seVolume = 1;

	//Pause中のフラグ
	bool isPause = false;
	//ボタン
	vector<Button> buttons;

	//カメラ座標
	Vector2D camPosition = Vector2D{ 0,0 };
	const Vector2D camPosOffset = Vector2D{ -WIN_WIDTH / 2,-WIN_HEIGHT / 2 };

	//自機
	LiveEntity player = {};

	//自機がフィールド外に出ないための壁
	vector<GameObject> edgeWall = {
		GameObject{Rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{Rect{-WIN_WIDTH / 2 + GAME_LINE,0,0,WIN_HEIGHT}},
	};
	//ブロック
	vector<Block> blocks = {};
	//パーティクル
	vector<Particle> particles = {};

	//ステージの難易度、区画の一番下まで到達するたびに１ずつ上がる
	unsigned int stageLevel = 0;

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


		if (nextScene == currentScene)
		{
			if (sceneTransitionProgress >= 10 && sceneTransitionProgress < 20)
			{
				sceneTransitionProgress++;
			}
			else
			{
				sceneTransitionProgress = 0;
			}
		}
		else
		{
			//シーン切り替えのカウントダウン
			sceneTransitionProgress++;
			if (sceneTransitionProgress >= 10) {
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

				gameui_->Reset();
				//ポーズ解除
				isPause = false;

				//レベルをリセット
				stageLevel = 0;

				//自機を初期座標へ
				player = LiveEntity{ RigidBody{ GameObject{ Rect{-WIN_WIDTH / 2 + GAME_LINE / 2,-WIN_HEIGHT / 2 - BLOCK_DIAMETER * 2,20,20},0,1,{0,-12}} },playerSprites };

				//地層生成
				GenerateLevel(blocks, stageLevel);
				break;
			}
			sceneInit = false;
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
					Button{Rect{GAME_LINE / 2, 160,150,50},"つづける","RESUME"},
					Button{Rect{GAME_LINE / 2, 460,150,50},"やめる","QUIT"},
				};
			}
			else if (player.isDespawned && gameui_->isrankEnd)
			{
				buttons = {
					Button{Rect{GAME_LINE / 4, 560,150,50},"リトライ","TRY AGAIN"},
					Button{Rect{GAME_LINE - (GAME_LINE / 4), 560,150,50},"タイトルへ","QUIT"},
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
			buttons[i].entity.position.y += sin(clock() / PI / 300 - i / 2.0) * 5;
			ButtonUpdate(buttons[i], mouseInputData, buttonPushSound);
		}

		switch (currentScene)
		{
		case logo:
			//ロゴ画面

			//曲を指定しない
			bgmNum = 0;

			//左クリックでタイトルへ
			if (!mouseInputData.click && mouseInputData.preClick)
			{
				nextScene = title;
			}
			break;
		case title:
			//タイトル画面

			//タイトルの曲を指定
			bgmNum = 1;

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

			//プレイパートの曲を指定
			bgmNum = 4;
			//曲の音量を大きく
			ChangeVolumeSoundMem(255, audioClip[2]);

			if (!isPause)
			{
				if (player.isLive)
				{
					if (player.rigidBody.gameObject.entity.position.y >= BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1))
					{
						//レベルアップ
						stageLevel++;

						//自機を上へ
						player.rigidBody.gameObject.entity.position.y = -WIN_HEIGHT / 2 - BLOCK_DIAMETER * 2;

						//地層生成
						GenerateLevel(blocks, stageLevel);
					}

					gameui_->Update();

					//全てのブロックを更新
					for (int i = 0; i < blocks.size(); i++)
					{
						blocks[i].rigidBody.gameObject.graphNum = blocksSprite[blocks[i].blockType];
						blocks[i].rigidBody.gameObject.graphLocalPos = blocks[i].rigidBody.gameObject.graphLocalPos * 0.3f;

						//落下
						blocks[i].rigidBody.gameObject.entity.position.y =
							min(blocks[i].rigidBody.gameObject.entity.position.y + BLOCK_DIAMETER / 10, BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1));

						unsigned int landingBlockIndex = min(blocks[i].landingBlockIndex, blocks.size() - 1);
						Vector2D pos = blocks[i].rigidBody.gameObject.entity.position;
						Vector2D targetPos = blocks[landingBlockIndex].rigidBody.gameObject.entity.position;
						if (pos.y >= BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1)
							|| (pos.x == targetPos.x
								&& pos.y < targetPos.y && pos.y > targetPos.y - BLOCK_DIAMETER))
						{
							//支えがある場合は軽量処理
							if (pos.y < BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1))
							{
								blocks[i].rigidBody.gameObject.entity.position.y = targetPos.y - BLOCK_DIAMETER;
							}
							blocks[i].latestLandingHeight = blocks[i].rigidBody.gameObject.entity.position.y;
							//壊れる準備が出来ている状態で壊せないブロックが着地していたら壊れる
							if (blocks[i].blockType == untappableblock || blocks[i].blockType == lethalblock)
							{
								if (blocks[i].status == "prepareBreak")
								{
									blocks[i].breaked = true;
								}
							}
						}
						else
						{
							//支えを無くしたら精密処理

							//下のブロックを探す
							for (int j = 0; j < blocks.size(); j++)
							{
								pos = blocks[i].rigidBody.gameObject.entity.position;
								targetPos = blocks[j].rigidBody.gameObject.entity.position;
								if (pos.x == targetPos.x
									&& pos.y < targetPos.y && pos.y > targetPos.y - BLOCK_DIAMETER)
								{
									blocks[i].rigidBody.gameObject.entity.position.y = targetPos.y - BLOCK_DIAMETER;
									blocks[i].landingBlockIndex = j;

									break;
								}
							}

							//壊せないブロック同士が接触したら壊れる準備
							if (blocks[i].blockType == untappableblock || blocks[i].blockType == lethalblock)
							{
								for (int j = 0; j < blocks.size(); j++)
								{
									pos = blocks[i].rigidBody.gameObject.entity.position;
									targetPos = blocks[j].rigidBody.gameObject.entity.position;
									if (i != j && blocks[i].rigidBody.gameObject.entity.position.y - blocks[i].latestLandingHeight > BLOCK_DIAMETER / 1.1f
										&& (blocks[j].blockType == untappableblock || blocks[j].blockType == lethalblock)
										&& VectorScale(pos - targetPos) <= BLOCK_DIAMETER * 1.2f)
									{
										blocks[i].status = "prepareBreak";
										blocks[j].status = "prepareBreak";
									}
								}
							}
						}

						if (blocks[i].blockType == sandblock)
						{
							//砂ブロックが自機に触れたら崩れる準備
							if (HitRectAndRect(blocks[i].rigidBody.gameObject.entity, player.rigidBody.gameObject.entity)
								&& VectorScale(blocks[i].rigidBody.gameObject.entity.position - player.rigidBody.gameObject.entity.position) <= BLOCK_DIAMETER)
							{
								blocks[i].status = "touched";
								blocks[i].rigidBody.gameObject.graphLocalPos =
									NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 1;
							}
							else
							{
								//崩れる準備が出来ている状態で砂ブロックが自機に触れていない状態が4フレーム（バッファのため）続いたら崩れる
								if (blocks[i].status == "touched")
								{
									blocks[i].status = "prepareBreak3";
								}
								else if (blocks[i].status == "prepareBreak3")
								{
									blocks[i].status = "prepareBreak2";
									blocks[i].rigidBody.gameObject.graphLocalPos =
										NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 4;
								}
								else if (blocks[i].status == "prepareBreak2")
								{
									blocks[i].status = "prepareBreak";
									blocks[i].rigidBody.gameObject.graphLocalPos =
										NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 4;
								}
								else if (blocks[i].status == "prepareBreak")
								{
									blocks[i].breaked = true;
								}
							}
						}

						//クリックされたら消す準備、壊せないブロックは少し演出を起こすだけ
						if (HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.position + (camPosition + camPosOffset))
							&& HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.pin + (camPosition + camPosOffset))
							&& !mouseInputData.click && mouseInputData.preClick)
						{
							if (blocks[i].blockType == untappableblock || blocks[i].blockType == lethalblock)
							{
								PlaySoundMem(reflectSound, DX_PLAYTYPE_BACK, true);
								blocks[i].rigidBody.gameObject.graphLocalPos =
									NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 10;
							}
							else
							{
								blocks[i].breaked = true;
							}
						}

						//リーサルブロックに自機が触れたら死ぬ
						if (blocks[i].blockType == lethalblock
							&& HitRectAndRect(blocks[i].rigidBody.gameObject.entity, player.rigidBody.gameObject.entity)
							&& VectorScale(blocks[i].rigidBody.gameObject.entity.position - player.rigidBody.gameObject.entity.position) <= BLOCK_DIAMETER)
						{
							player.isLive = false;
						}
					}
					//消す準備が出来たブロックを全部消す
					for (int i = 0; i < blocks.size(); i++)
					{
						if (blocks[i].breaked)
						{
							gameui_->blockCountT1++;

							//破片に貼り付ける画像を決める
							int* shardGraph;
							switch (blocks[i].blockType)
							{
							case weakblock:
								shardGraph = weakBlockShards;
								break;
							case sandblock:
								shardGraph = sandBlockShards;
								break;
							case frameblock:
								shardGraph = frameBlockShards;
								break;
							case untappableblock:
								shardGraph = unTappableBlockShards;
								break;
							case lethalblock:
								shardGraph = lethalBlockShards;
								break;
							default:
								shardGraph = frameBlockShards;
								break;
							}
							//破片を生成
							float rotRand = rand();
							for (int j = 0; j < 9; j++)
							{
								float angleRad = PI * 2 / 9 * j + rotRand;
								int spriteIndex = rand() % 4;
								if (j % 2 == 0)
								{
									spriteIndex += 4;
								}
								particles.push_back(Particle{ RigidBody{GameObject{
									blocks[i].rigidBody.gameObject.entity,shardGraph[spriteIndex]},
									Vector2D{sinf(angleRad),cosf(angleRad)} *15},
									0.5f + (rand() % 20) * 0.01f,(float)(rand() % 7 - 3) * 0.1f });
							}

							PlaySoundMem(blockBreakSound, DX_PLAYTYPE_BACK, true);
							blocks.erase(blocks.begin() + i);
							i--;
						}
					}

					gameui_->depthT1 =
						stageLevel * PLAYPART_HEIGHT + max((player.rigidBody.gameObject.entity.position.y) / BLOCK_DIAMETER + 1, 0);

					//ボタンを押した時の処理
					if (IsButtonClicked(buttons, 0))
					{
						//ポーズする
						isPause = true;
					}
				}
				else if (player.isDespawned)
				{
					//自機死亡時
					//続ける
					if (IsButtonClicked(buttons, 0))
					{
						//リセット
						sceneInit = true;
					}
					//終わる
					if (IsButtonClicked(buttons, 1))
					{
						//タイトルへ
						nextScene = title;
					}

					//曲の音量を小さく
					ChangeVolumeSoundMem(128, audioClip[2]);
				}

				//全てのパーティクルを更新
				for (int i = 0; i < particles.size(); i++)
				{
					RigidBodyUpdate(particles[i].rigidBody, { 0,0 }, { particles[i].drag,particles[i].drag });
					particles[i].rigidBody.gameObject.rot += particles[i].torque;
					particles[i].rigidBody.gameObject.graphScale = powf((float)particles[i].lifetime / particles[i].maxLifeTime, 0.4f);
					particles[i].lifetime--;
				}
				//寿命が尽きたパーティクルを消す
				for (int i = 0; i < particles.size(); i++)
				{
					if (particles[i].lifetime <= 0)
					{
						particles.erase(particles.begin() + i);
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
				LiveEntityUpdate(&player, liveEntityWalls, damageSound, despawnSound, despawnShards, particles);

				//カメラ追従
				camPosition = Vector2D{ 0,player.rigidBody.gameObject.entity.position.y };

				//両端の壁を追従させる
				edgeWall = {
					GameObject{Rect{Vector2D{-WIN_WIDTH / 2,0} + camPosition,{0,WIN_HEIGHT}}},
					GameObject{Rect{Vector2D{-WIN_WIDTH / 2 + GAME_LINE,0} + camPosition,{0,WIN_HEIGHT}}},
				};
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

				//曲の音量を小さく
				ChangeVolumeSoundMem(128, audioClip[2]);
			}

			break;
		case credit:
			//クレジット画面

			//クレジットの曲を指定
			bgmNum = 3;

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		case howtoplay:
			//遊び方説明画面

			//遊び方説明の曲を指定
			bgmNum = 2;

			//ボタンを押した時の処理
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		}

		//曲を再生、同時に流すべきでない曲を止める
		if (!CheckSoundMem(audioClip[bgmNum])) {
			PlaySoundMem(audioClip[bgmNum], DX_PLAYTYPE_BACK, true);
		}
		for (int i = 0; i < sizeof(audioClip) / sizeof(*audioClip); i++) {
			if (i != bgmNum) {
				StopSoundMem(audioClip[i]);
			}
		}

		// 描画処理

		// 画面の背景色を設定する
		SetBackgroundColor(0xff, 0xd8, 0x86);

		//背景画像
		int currentBGGraph = bgGraph;

		if (currentScene == credit)
		{
			SetBackgroundColor(0x86, 0xc7, 0xff);
			currentBGGraph = optionBGGraph;
		}

		DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2 - fmodf(clock() * 0.00005f,1) * 320 + 160, 1, 0, currentBGGraph, TRUE);

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
				WIN_WIDTH / 7 * 3, WIN_HEIGHT / 3 * 2,
				"TEAM 4005",
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
			//権利表示
			DrawString(
				WIN_WIDTH / 3, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "2024 ALHA TERAPETA GAMES / TEAM 4005",
				GetColor(0, 0, 0));
			//バージョン表示
			DrawString(
				WIN_WIDTH / 6 * 5, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "Ver. 1.0.0",
				GetColor(0, 0, 0));
			break;
		case playpart:
			//プレイパート

			//まずは背景を描画(スクロール付き)
			DrawRotaGraph(GAME_LINE / 2, WIN_HEIGHT / 2 - fmodf(camPosition.y + 640, 320) + 160, 1, 0, playPartBGGraph, true);
			//全てのブロックを描画
			for (int i = 0; i < blocks.size(); i++) {
				RenderObject(blocks[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//全てのパーティクルを描画
			for (int i = 0; i < particles.size(); i++) {
				RenderObject(particles[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//自機を描画
			if (player.despawnAnimProgress > 0.5f)
			{
				RenderObject(player.rigidBody.gameObject, camPosition + camPosOffset);
			}


			//このラインからはUIゾーンなのでいっそここにボックスUIおいてもいいや
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, 0xffffff, TRUE);
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xd8, 0x86), FALSE);
			DrawTriangle(
				WIN_WIDTH,0,
				WIN_WIDTH,WIN_HEIGHT,
				GAME_LINE,WIN_HEIGHT,
				GetColor(0xff, 0xd8, 0x86), TRUE);
			gameui_->Draw();
			if (player.isDespawned)
			{
				gameui_->DrawRank();
			}
			break;
		case credit:
			//クレジット画面
			DrawString(
				WIN_WIDTH / 4, 0,
				"\n\nプログラマー\n　鰯ユウ\n　神無月\n\nチーフプログラマー\n　てらぺた\n\nエフェクト\n　てらぺた\n\nキャラクターデザイン\n　あるは\n\nアート\n　あるは\n\nサウンドエフェクト\n　あるは\n\nコンポーサー\n　あるは\n\nUIデザイン\n　神無月\n　てらぺた",
				GetColor(0, 0, 0));
			DrawString(
				WIN_WIDTH / 2, 0,
				"\n\nテクニカルサポート\n　鰯ユウ\n　神無月\n\nビジュアルアドバイザー\n　てらぺた\n\nエグゼクティブプロデューサー\n　てらぺた\n\nディレクター\n　てらぺた\n\nかいはつ\n　あるはてらぺたゲームズ\n　チーム4005\n\n\nALHA TERAPETA GAMES / TEAM 4005\nAll Rights Reserved.",
				GetColor(0, 0, 0));
			break;
		case howtoplay:
			DrawRotaGraph(WIN_WIDTH / 4, WIN_HEIGHT / 2, 1, 0, tutorialGraph0, true);
			DrawRotaGraph(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 2, 1, 0, tutorialGraph1, true);
			break;
		}

		//全てのボタンを描画
		for (int i = 0; i < buttons.size(); i++)
		{
			DrawButton(buttons[i]);
		}

		//シーン遷移シャッター
		DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2,
			WIN_WIDTH / 64 * sinf(sceneTransitionProgress / 20.0f * PI),
			(sceneTransitionProgress - 10)/10.0f, blocksSprite[0], true);

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
