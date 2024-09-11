#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>

class gameUi
{
public:

	void Reset();

	void Initialize();

	void Update();

	void Draw();

	//常に動いてるタイマー
	int autotimer = 0;
	//1の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT1 = 0;
	//10の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT10 = 0;
	//100の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT100 = 0;
	//100の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT1000 = 0;
	//ここまで
	////////////////////////////////////////////////////

	/////////////////////////////////////////////
	////////深層何mまですすんでいるか?のUI////////////
	////////////////////////////////////////////
	// 	//常に動いてるタイマー
	//1の位
	int depthT1 = 0;
	//10の位
	int depthT10 = 0;
	//100の位
	int depthT100 = 0;
	//1000の位
	int depthT1000 = 0;
	//ここまで
	////////////////////////////////////////////////////

	/////////////////////////////////////////////
	//////何個ブロックを壊しているか？のUI////////////
	////////////////////////////////////////////
	//常に動いてるタイマー
	//1の位
	int blockCountT1 = 0;
	//10の位
	int blockCountT10 = 0;
	//100の位
	int blockCountT100 = 0;
	//1000の位
	int blockCountT1000 = 0;
	//ここまで
	////////////////////////////////////////////////////

	//総合評価用タイマー
	int rankTimer = 0;
	//ランキング清算後ボタンを出すためのフラグ
	bool isrankEnd = false;
	
	//ランクの描画
	void DrawRank();
private:
	//深層何メートルまで進んでるかのUI描画(桁数、X軸)
	void DrawUI(int digTimerRank, int uiPosX, int uiposY);
	
private:
	//////////////////////////
	////残り時間を示すタイマー////
	/////////////////////////
	int zeroGraph;
	int oneGraph;
	int twoGraph;
	int threeGraph;
	int fourGraph;
	int fiveGraph;
	int sixGraph;
	int sevenGraph;
	int eightGraph;
	int nineGraph;

	int scoredepGraph;
	int scorebloGraph;
	int scorelastGraph;
};