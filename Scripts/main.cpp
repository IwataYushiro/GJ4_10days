#include "DxLib.h"
#include "KxLib.h"

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "xx2x_xx_ナマエ: タイトル";

// ウィンドウ横幅
const int WIN_WIDTH = 1280;

// ウィンドウ縦幅
const int WIN_HEIGHT = 720;

struct Player
{
	RigidBody rigidBody;
	bool direction;
	int stuckFrameCount;
};

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
	//タイトル画面(タイトル画面からシューティングゲーム)とBGM
	int titleGraph;
	titleGraph = LoadGraph("Resources/Textures/title.png");
	//ゲームシーン画面とBGM
	int gameUiGraph;
	gameUiGraph = LoadGraph("Resources/Textures/gemeui.png");
	//クリア画面(シューティングゲームからクリア画面)とBGM
	int clearGraph;
	clearGraph = LoadGraph("Resources/Textures/clear.png");

	// ゲームループで使う変数の宣言

	//シーン管理
	int title = 0;
	int gamemode = 1;
	int clear = 2;
	int sceneNo = title;
	
	//シーン用のタイマー
	int sceneTimer[3] = {0};


	const Vector2D mapScale = {8,100};

	const int playerSprite = LoadGraph("Resources/Textures/napnose.png");


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
		//シーン移行管理
		if (keys[KEY_INPUT_RETURN] == 1) {
			if (sceneNo == title) {
				sceneTimer[title]++;
				sceneTimer[clear] = 0;
			}
			else if (sceneNo == gamemode) {
				sceneTimer[gamemode]++;
				sceneTimer[title] = 0;
			}
			else if (sceneNo == clear) {
				sceneTimer[clear]++;
				sceneTimer[gamemode] = 0;
			}
		}
		if (sceneTimer[title] >= 30) {
			sceneNo = gamemode;
		}
		else if (sceneTimer[gamemode] >= 30) {
			sceneNo = clear;
		}
		else if (sceneTimer[clear] >= 30) {
			sceneNo = title;
		}



		//プレイヤーの物理挙動
		RigidBodyBehaviour(player.rigidBody, {0,1}, {0.5,1}, edgeWall);
		//プレイヤーが着地していたら
		if (player.rigidBody.landing)
		{
			//しばらく前方に進めなければ反転
			if ((player.direction
				&& player.rigidBody.gameObject.beforePos.x >= player.rigidBody.gameObject.entity.x)
				|| (!player.direction
				&& player.rigidBody.gameObject.beforePos.x <= player.rigidBody.gameObject.entity.x))
			{
				player.stuckFrameCount++;
			}
			else
			{
				player.stuckFrameCount--;
			}
			player.stuckFrameCount = min(max(0, player.stuckFrameCount), 3);

			if (player.stuckFrameCount >= 3)
			{
				player.direction = !player.direction;
				player.stuckFrameCount = 0;
			}

			//前進
			float playerMoveForce = 3;
			if (!player.direction)
			{
				playerMoveForce *= -1;
			}
			player.rigidBody.movement.x += playerMoveForce;
		}
		else
		{
			player.stuckFrameCount = 0;
		}

		// 描画処理
		if (sceneNo == 0) {
			DrawGraph(0, 0, titleGraph, true);
		}
		else if (sceneNo == 1) {
			DrawGraph(0, 0, gameUiGraph, true);
		}
		else if (sceneNo == 2) {
			DrawGraph(0, 0, clearGraph, true);
		}

		DrawFormatString(0, 0, GetColor(122, 122, 122), "sceneTimer[title] : %d", sceneTimer[title]);
		DrawFormatString(0, 30, GetColor(122, 122, 122), "sceneTimer[gamemode] : %d", sceneTimer[gamemode]);
		DrawFormatString(0, 60, GetColor(122, 122, 122), "sceneTimer[clear] : %d", sceneTimer[clear]);


		RenderObject(player.rigidBody.gameObject, Vector2D{ -WIN_WIDTH / 2,WIN_HEIGHT / 2 });

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
