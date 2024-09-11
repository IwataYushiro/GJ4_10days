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
	//�r�b�g�}�b�v�t�H���g�p�̐����摜�ƃ^�C�}�[
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

	//10�̌�
	timerT10 = timerT1 / 10;
	//100�̌�
	timerT100 = timerT10 / 10;
	//1000�̌�
	timerT1000 = timerT100 / 10;

	//�ǂ��܂Ō@������
	//10�̌�
	depthT10 = depthT1 / 10;
	//100�̌�
	depthT100 = depthT10 / 10;
	//1000�̌�
	depthT1000 = depthT100 / 10;

	//�ǂ��܂Ńu���b�N���󂵂���
	//10�̌�
	blockCountT10 = blockCountT1 / 10;
	//100�̌�
	blockCountT100 = blockCountT10 / 10;
	//1000�̌�
	blockCountT1000 = blockCountT100 / 10;
}

void gameUi::Draw()
{
	//�r�b�g�}�b�v�t�H���g�^�C�}�[
	//�^�C�}�[
	DrawUI(timerT1, 1050, 60);
	//10�̈�
	DrawUI(timerT10, 1000, 60);
	//100�̈�
	DrawUI(timerT100, 950, 60);
	//1000�̈�
	DrawUI(timerT1000, 900, 60);

	DrawString(950, 30, "TIME", GetColor(0, 0, 0));
	DrawFormatString(1150, 155, GetColor(0, 0, 0), "%d", autotimer);
	
	//�c��[�w��m����UI
	//1�̈�
	DrawUI(depthT1, 1050, 290);
	//10�̈�
	DrawUI(depthT10, 1000, 290);
	//100�̈�
	DrawUI(depthT100, 950, 290);
	//1000�̈�
	DrawUI(depthT1000, 900, 290);

	DrawString(950, 260, "DEPTH", GetColor(0, 0, 0));
	DrawString(1150, 385, "m", GetColor(0, 0, 0));
	//�[�w�l��9999���J���X�g
	if (depthT1 >= 10)
	{
		DrawString(1150, 350, "+", GetColor(0, 0, 0));
	}

	//�u���b�N�������󂵂����̂���UI
	//1�̈�
	DrawUI(blockCountT1, 1050, 550);
	//10�̈�
	DrawUI(blockCountT10, 1000, 550);
	//100�̈�
	DrawUI(blockCountT100, 950, 550);
	//1000�̈�
	DrawUI(blockCountT1000, 900, 550);

	DrawString(950, 500, "�󂵂��u���b�N��", GetColor(0, 0, 0));
	DrawString(1150, 625, "��", GetColor(0, 0, 0));
	//�u���b�N�j��J�E���g��9999���J���X�g
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
