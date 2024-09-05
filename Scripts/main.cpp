#include "DxLib.h"
#include "KxLib.h"

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "GJ4_Gamejam";

// ウィンドウ横幅
const int WIN_WIDTH = 1280;
//UIライン
const int GAME_LINE = 960;

// ウィンドウ縦幅
const int WIN_HEIGHT = 720;

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

void PlayerBehaviour(Player* player, std::vector<GameObject> blocks)
{
	//プレイヤーの物理挙動
	RigidBodyBehaviour(player->rigidBody, { 0,1 }, { 0.5,1 }, blocks);
	//プレイヤーが着地していたら
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
		float playerMoveForce = 3;
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
	//ゲームシーン画面とBGM
	//const int gameUiGraph = LoadGraph("Resources/Textures/gemeui.png");
	//クリア画面(シューティングゲームからクリア画面)とBGM
	const int clearGraph = LoadGraph("Resources/Textures/clear.png");
	//自機
	const int playerSprite = LoadGraph("Resources/Textures/napnose.png");

	// ゲームループで使う変数の宣言

	//現在のシーン
	Scene currentScene = logo;
	//遷移使用としている次のシーン
	Scene nextScene = logo;
	
	//シーン用のタイマー
	float sceneTransitionProgress = 0;

	const Vector2D mapScale = {8,100};

	Player player = Player{ RigidBody{ GameObject{ rect{0,0,64,64}, playerSprite} } };
	vector<GameObject> edgeWall = {
		GameObject{rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{rect{WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
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
		
		//シーン遷移入力で対応するシーンへの遷移を準備
		if (keys[KEY_INPUT_RETURN] == 1)
		{
			switch (currentScene)
			{
			case title:
				nextScene = playpart;
				break;
			default:
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
				player = Player{ RigidBody{ GameObject{ rect{0,0,64,64}, playerSprite} } };
				break;
			default:
				break;
			}
		}

		switch (currentScene)
		{
		case title:
			//タイトル画面
			break;
		case playpart:
			//プレイパート

			//自機を更新
			PlayerBehaviour(&player, edgeWall);
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
		}
		else if (sceneNo == 1) {

			//このラインからはUIゾーンなのでいっそここにボックスUIおいてもいいや
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);
		}
		else if (sceneNo == 2) {
			DrawGraph(0, 0, clearGraph, true);
		}

		DrawFormatString(0, 0, GetColor(122, 122, 122), "sceneTimer[title] : %d", sceneTimer[title]);
		DrawFormatString(0, 30, GetColor(122, 122, 122), "sceneTimer[gamemode] : %d", sceneTimer[gamemode]);
		DrawFormatString(0, 60, GetColor(122, 122, 122), "sceneTimer[clear] : %d", sceneTimer[clear]);

		if (sceneNo == gamemode) {
			RenderObject(player.rigidBody.gameObject, Vector2D{ -WIN_WIDTH / 2,WIN_HEIGHT / 2 });
			break;
		case logo:
			//ロゴ画面
			DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 2, 0, logoGraph, 0);
			break;
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
