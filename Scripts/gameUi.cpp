#include "gameUi.h"

void gameUi::Reset()
{
	autotimer = 0;
	timerT1 = 0;
	timerT10 = 0;
	timerT100 = 0;
	timerT1000 = 0;

	depthT1 = 0;
	depthT10 = 0;
	depthT100 = 0;
	depthT1000 = 0;

	blockCountT1 = 0;
	blockCountT10 = 0;
	blockCountT100 = 0;
	blockCountT1000 = 0;

	rankTimer = 0; 
	isrankEnd = false;
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

	//10の桁
	timerT10 = timerT1 / 10;
	//100の桁
	timerT100 = timerT10 / 10;
	//1000の桁
	timerT1000 = timerT100 / 10;

	//どこまで掘ったか
	//10の桁
	depthT10 = depthT1 / 10;
	//100の桁
	depthT100 = depthT10 / 10;
	//1000の桁
	depthT1000 = depthT100 / 10;

	//どこまでブロックを壊したか
	//10の桁
	blockCountT10 = blockCountT1 / 10;
	//100の桁
	blockCountT100 = blockCountT10 / 10;
	//1000の桁
	blockCountT1000 = blockCountT100 / 10;
}

void gameUi::Draw()
{
	//ビットマップフォントタイマー
	//タイマー
	DrawUI(timerT1, 1050, 60);
	//10の位
	DrawUI(timerT10, 1000, 60);
	//100の位
	DrawUI(timerT100, 950, 60);
	//1000の位
	DrawUI(timerT1000, 900, 60);

	DrawString(950, 30, "TIME", GetColor(0, 0, 0));
	DrawFormatString(1150, 155, GetColor(0, 0, 0), "%d", autotimer);
	
	//残り深層何mかのUI
	//1の位
	DrawUI(depthT1, 1050, 290);
	//10の位
	DrawUI(depthT10, 1000, 290);
	//100の位
	DrawUI(depthT100, 950, 290);
	//1000の位
	DrawUI(depthT1000, 900, 290);

	DrawString(950, 260, "DEPTH", GetColor(0, 0, 0));
	DrawString(1150, 385, "m", GetColor(0, 0, 0));
	//深層値は9999がカンスト
	if (depthT1 >= 10)
	{
		DrawString(1150, 350, "+", GetColor(0, 0, 0));
	}

	//ブロックをいくつ壊したかのかのUI
	//1の位
	DrawUI(blockCountT1, 1050, 550);
	//10の位
	DrawUI(blockCountT10, 1000, 550);
	//100の位
	DrawUI(blockCountT100, 950, 550);
	//1000の位
	DrawUI(blockCountT1000, 900, 550);

	DrawString(950, 500, "壊したブロック数", GetColor(0, 0, 0));
	DrawString(1150, 625, "個", GetColor(0, 0, 0));
	//ブロック破壊カウントも9999がカンスト
	if (blockCountT1 >= 10)
	{
		DrawString(1150, 590, "+", GetColor(0, 0, 0));
	}
}

void gameUi::DrawUI(int digTimerRank, int uiPosX, int uiPosY)
{
	if (digTimerRank % 10 <= 0) { DrawGraph(uiPosX, uiPosY, zeroGraph, true); }
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

void gameUi::DrawRank()
{
	if (rankTimer>=20)
	{
		DrawString(1200, 290, "A", GetColor(0, 0, 0));
	}
	if (rankTimer >= 40)
	{
		DrawString(1200, 550, "B", GetColor(0, 0, 0));
	}
	if (rankTimer >= 60)
	{
		DrawString(450, 360, "S", GetColor(255, 255, 255));
	}
	if (rankTimer >= 100)
	{
		isrankEnd = true;
	}
	rankTimer++;
}
