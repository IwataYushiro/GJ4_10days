#include "gameUi.h"

void gameUi::Reset()
{
	autotimer = 0;
	timerT1 = 0;
	timerT2 = 0;
	timerT3 = 0;

	digTimerT1 = 0;
	digTimerT2 = 0;

	blockCountT1 = 0;
	blockCountT2 = 0;
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
	//10の桁
	digTimerT2 = digTimerT1 / 10;
	//100の桁
	digTimerT100 = digTimerT2 / 10;
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
	DrawDigUI(digTimerT1, 1050);
	//10の位
	DrawDigUI(digTimerT2, 1000);
	//100の位
	DrawDigUI(digTimerT100, 950);

	//ブロックをいくつ壊したかのかのUI
	//1の位
	if (blockCountT1 <= 0) { DrawGraph(1050, 550, zeroGraph, true); }
	else if (blockCountT1 == 1) { DrawGraph(1050, 550, oneGraph, true); }
	else if (blockCountT1 == 2) { DrawGraph(1050, 550, twoGraph, true); }
	else if (blockCountT1 == 3) { DrawGraph(1050, 550, threeGraph, true); }
	else if (blockCountT1 == 4) { DrawGraph(1050, 550, fourGraph, true); }
	else if (blockCountT1 == 5) { DrawGraph(1050, 550, fiveGraph, true); }
	else if (blockCountT1 == 6) { DrawGraph(1050, 550, sixGraph, true); }
	else if (blockCountT1 == 7) { DrawGraph(1050, 550, sevenGraph, true); }
	else if (blockCountT1 == 8) { DrawGraph(1050, 550, eightGraph, true); }
	else if (blockCountT1 == 9) { DrawGraph(1050, 550, nineGraph, true); }
	//10の位
	if (blockCountT2 == 0) { DrawGraph(1000, 550, zeroGraph, true); }
	else if (blockCountT2 == 1) { DrawGraph(1000, 550, oneGraph, true); }
	else if (blockCountT2 == 2) { DrawGraph(1000, 550, twoGraph, true); }
	else if (blockCountT2 == 3) { DrawGraph(1000, 550, threeGraph, true); }
	else if (blockCountT2 == 4) { DrawGraph(1000, 550, fourGraph, true); }
	else if (blockCountT2 == 5) { DrawGraph(1000, 550, fiveGraph, true); }
	else if (blockCountT2 == 6) { DrawGraph(1000, 550, sixGraph, true); }
	else if (blockCountT2 == 7) { DrawGraph(1000, 550, sevenGraph, true); }
	else if (blockCountT2 == 8) { DrawGraph(1000, 550, eightGraph, true); }
	else if (blockCountT2 == 9) { DrawGraph(1000, 550, nineGraph, true); }

}

void gameUi::DrawDigUI(int digTimerRank, int uiPosX)
{
	if (digTimerRank % 10 <= 0) { DrawGraph(uiPosX, 290, zeroGraph, true); }
	else if (digTimerRank % 10 == 1) { DrawGraph(uiPosX, 290, oneGraph, true); }
	else if (digTimerRank % 10 == 2) { DrawGraph(uiPosX, 290, twoGraph, true); }
	else if (digTimerRank % 10 == 3) { DrawGraph(uiPosX, 290, threeGraph, true); }
	else if (digTimerRank % 10 == 4) { DrawGraph(uiPosX, 290, fourGraph, true); }
	else if (digTimerRank % 10 == 5) { DrawGraph(uiPosX, 290, fiveGraph, true); }
	else if (digTimerRank % 10 == 6) { DrawGraph(uiPosX, 290, sixGraph, true); }
	else if (digTimerRank % 10 == 7) { DrawGraph(uiPosX, 290, sevenGraph, true); }
	else if (digTimerRank % 10 == 8) { DrawGraph(uiPosX, 290, eightGraph, true); }
	else if (digTimerRank % 10 == 9) { DrawGraph(uiPosX, 290, nineGraph, true); }
}
