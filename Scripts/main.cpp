#include "DxLib.h"

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "xx2x_xx_ナマエ: タイトル";

// ウィンドウ横幅
const int WIN_WIDTH = 1280;

// ウィンドウ縦幅
const int WIN_HEIGHT = 720;

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
