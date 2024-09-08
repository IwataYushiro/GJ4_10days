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

	//常に動いてるタイマー
	int autotimer = 0;
	//1の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT1 = 0;
	//10の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT2 = 0;
	//100の位のタイマー(autotimerが60カウントになるごとに1変動)
	int timerT3 = 0;
	//ここまで
	////////////////////////////////////////////////////

	/////////////////////////////////////////////
	////////深層何mまですすんでいるか?のUI////////////
	////////////////////////////////////////////
	// 	//常に動いてるタイマー
	//1の位
	int digTimerT1 = 0;
	//10の位
	int digTimerT2 = 0;

};