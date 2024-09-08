#include "gameUi.h"

void gameUi::Reset()
{
	autotimer = 0;
	timerT1 = 0;
	timerT2 = 0;
	timerT3 = 0;
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
		timerT1 = 0;
	}

}

void gameUi::Draw()
{
	//ビットマップフォントタイマー
	if (timerT1 == 0) { DrawGraph(950, 60, zeroGraph, true); }
	else if (timerT1 == 1) { DrawGraph(950, 60, oneGraph, true); }
	else if (timerT1 == 2) { DrawGraph(950, 60, twoGraph, true); }
	else if (timerT1 == 3) { DrawGraph(950, 60, threeGraph, true); }
	else if (timerT1 == 4) { DrawGraph(950, 60, fourGraph, true); }
	else if (timerT1 == 5) { DrawGraph(950, 60, fiveGraph, true); }
	else if (timerT1 == 6) { DrawGraph(950, 60, sixGraph, true); }
	else if (timerT1 == 7) { DrawGraph(950, 60, sevenGraph, true); }
	else if (timerT1 == 8) { DrawGraph(950, 60, eightGraph, true); }
	else if (timerT1 == 9) { DrawGraph(950, 60, nineGraph, true); }
}
