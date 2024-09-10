#include "gameUi.h"

void gameUi::Reset()
{
	autotimer = 0;
	timerT1 = 0;
	timerT2 = 0;
	timerT3 = 0;

	depthT1 = 0;
	depthT10 = 0;
	depthT100 = 0;

	blockCountT1 = 0;
	blockCountT10 = 0;
	blockCountT100 = 0;
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
	if (timerT2 >= 10) {
		timerT3++;
		timerT2 = 0;
	}

	//どこまで掘ったか
	//10の桁
	depthT10 = depthT1 / 10;
	//100の桁
	depthT100 = depthT10 / 10;
	//どこまでブロックを壊したか
	//10の桁
	blockCountT10 = blockCountT1 / 10;
	//100の桁
	blockCountT100 = blockCountT10 / 10;
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
	//100の位
	if (timerT3 == 0) { DrawGraph(950, 60, zeroGraph, true); }
	else if (timerT3 == 1) { DrawGraph(950, 60, oneGraph, true); }
	else if (timerT3 == 2) { DrawGraph(950, 60, twoGraph, true); }
	else if (timerT3 == 3) { DrawGraph(950, 60, threeGraph, true); }
	else if (timerT3 == 4) { DrawGraph(950, 60, fourGraph, true); }
	else if (timerT3 == 5) { DrawGraph(950, 60, fiveGraph, true); }
	else if (timerT3 == 6) { DrawGraph(950, 60, sixGraph, true); }
	else if (timerT3 == 7) { DrawGraph(950, 60, sevenGraph, true); }
	else if (timerT3 == 8) { DrawGraph(950, 60, eightGraph, true); }
	else if (timerT3 == 9) { DrawGraph(950, 60, nineGraph, true); }

	//残り深層何mかのUI
	//1の位
	DrawUI(depthT1, 1050, 290);
	//10の位
	DrawUI(depthT10, 1000, 290);
	//100の位
	DrawUI(depthT100, 950, 290);

	//ブロックをいくつ壊したかのかのUI
	//1の位
	DrawUI(blockCountT1, 1050, 550);
	//10の位
	DrawUI(blockCountT10, 1000, 550);
	//10の位
	DrawUI(blockCountT100, 950, 550);
}

void gameUi::DrawUI(int digTimerRank, int uiPosX, int uiPosY)
{
	if (digTimerRank % 10 <= 0) { DrawGraph(uiPosX, uiPosY, zeroGraph, true);}
	else if (digTimerRank % 10 == 1) { DrawGraph(uiPosX, uiPosY, oneGraph, true); }
	else if (digTimerRank % 10 == 2) { DrawGraph(uiPosX, uiPosY, twoGraph, true); }
	else if (digTimerRank % 10 == 3) { DrawGraph(uiPosX, uiPosY, threeGraph, true); }
	else if (digTimerRank % 10 == 4) { DrawGraph(uiPosX, uiPosY, fourGraph, true); }
	else if (digTimerRank % 10 == 5) { DrawGraph(uiPosX, uiPosY, fiveGraph, true); }
	else if (digTimerRank % 10 == 6) { DrawGraph(uiPosX, uiPosY, sixGraph, true); }
	else if (digTimerRank % 10 == 7) { DrawGraph(uiPosX, uiPosY, sevenGraph, true); }
	else if (digTimerRank % 10 == 8) { DrawGraph(uiPosX, uiPosY, eightGraph, true); }
	else if (digTimerRank % 10 == 9) { DrawGraph(uiPosX, uiPosY, nineGraph, true); }
}
