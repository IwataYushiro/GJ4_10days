#include "gameUi.h"

void gameUi::Reset()
{
	autotimer = 0;
	timerT1 = 0;
	timerT2 = 0;
	timerT3 = 0;

	digTimerT1 = 0;
	digTimerT2 = 0;
}

void gameUi::Initialize()
{
	//ビットマップフォント用の数字画像とタイマー
	zeroGraph = LoadGraph("Resources/Textures/timer0.png");
	oneGraph = LoadGraph("Resources/Textures/timer1.png");
	twoGraph = LoadGraph("Resources/Textures/timer2.png");
	threeGraph = LoadGraph("Resources/Textures/timer3.png");
	fourGraph = LoadGraph("Resources/Textures/timer4.png");
	fiveGraph = LoadGraph("Resources/Textures/timer5.png");
	sixGraph = LoadGraph("Resources/Textures/timer6.png");
	sevenGraph = LoadGraph("Resources/Textures/timer7.png");
	eightGraph = LoadGraph("Resources/Textures/timer8.png");
	nineGraph = LoadGraph("Resources/Textures/timer9.png");
}

void gameUi::Update()
{
	autotimer++;
	if (autotimer >= 58) {
		timerT1++;
		autotimer = 0;
	}
	if (timerT1 >= 10) {
		timerT2++;
		timerT1 = 0;
	}
	
	//どこまで掘ったか
	if (digTimerT1 >= 10) {
		digTimerT2++;
		digTimerT1 = 0;
	}
}

void gameUi::Draw()
{
	//ビットマップフォントタイマー
	//タイマー
	//1の位
	if (timerT1 == 0) { DrawGraph(1050, 60, zeroGraph, true); }
	else if (timerT1 == 1) { DrawGraph(1050, 60, oneGraph, true); }
	else if (timerT1 == 2) { DrawGraph(1050, 60, twoGraph, true); }
	else if (timerT1 == 3) { DrawGraph(1050, 60, threeGraph, true); }
	else if (timerT1 == 4) { DrawGraph(1050, 60, fourGraph, true); }
	else if (timerT1 == 5) { DrawGraph(1050, 60, fiveGraph, true); }
	else if (timerT1 == 6) { DrawGraph(1050, 60, sixGraph, true); }
	else if (timerT1 == 7) { DrawGraph(1050, 60, sevenGraph, true); }
	else if (timerT1 == 8) { DrawGraph(1050, 60, eightGraph, true); }
	else if (timerT1 == 9) { DrawGraph(1050, 60, nineGraph, true); }
	//10の位
	if (timerT2 == 0) { DrawGraph(1000, 60, zeroGraph, true); }
	else if (timerT2 == 1) { DrawGraph(1000, 60, oneGraph, true); }
	else if (timerT2 == 2) { DrawGraph(1000, 60, twoGraph, true); }
	else if (timerT2 == 3) { DrawGraph(1000, 60, threeGraph, true); }
	else if (timerT2 == 4) { DrawGraph(1000, 60, fourGraph, true); }
	else if (timerT2 == 5) { DrawGraph(1000, 60, fiveGraph, true); }
	else if (timerT2 == 6) { DrawGraph(1000, 60, sixGraph, true); }
	else if (timerT2 == 7) { DrawGraph(1000, 60, sevenGraph, true); }
	else if (timerT2 == 8) { DrawGraph(1000, 60, eightGraph, true); }
	else if (timerT2 == 9) { DrawGraph(1000, 60, nineGraph, true); }


	//残り深層何mかのUI
	//1の位
	if (digTimerT1 <= 0) { DrawGraph(1050, 350, zeroGraph, true); }
	else if (digTimerT1 == 1) { DrawGraph(1050, 350, oneGraph, true); }
	else if (digTimerT1 == 2) { DrawGraph(1050, 350, twoGraph, true); }
	else if (digTimerT1 == 3) { DrawGraph(1050, 350, threeGraph, true); }
	else if (digTimerT1 == 4) { DrawGraph(1050, 350, fourGraph, true); }
	else if (digTimerT1 == 5) { DrawGraph(1050, 350, fiveGraph, true); }
	else if (digTimerT1 == 6) { DrawGraph(1050, 350, sixGraph, true); }
	else if (digTimerT1 == 7) { DrawGraph(1050, 350, sevenGraph, true); }
	else if (digTimerT1 == 8) { DrawGraph(1050, 350, eightGraph, true); }
	else if (digTimerT1 == 9) { DrawGraph(1050, 350, nineGraph, true); }
	//10の位
	if (digTimerT2 == 0) { DrawGraph(1000, 350, zeroGraph, true); }
	else if (digTimerT2 == 1) { DrawGraph(1000, 350, oneGraph, true); }
	else if (digTimerT2 == 2) { DrawGraph(1000, 350, twoGraph, true); }
	else if (digTimerT2 == 3) { DrawGraph(1000, 350, threeGraph, true); }
	else if (digTimerT2 == 4) { DrawGraph(1000, 350, fourGraph, true); }
	else if (digTimerT2 == 5) { DrawGraph(1000, 350, fiveGraph, true); }
	else if (digTimerT2 == 6) { DrawGraph(1000, 350, sixGraph, true); }
	else if (digTimerT2 == 7) { DrawGraph(1000, 350, sevenGraph, true); }
	else if (digTimerT2 == 8) { DrawGraph(1000, 350, eightGraph, true); }
	else if (digTimerT2 == 9) { DrawGraph(1000, 350, nineGraph, true); }
}
