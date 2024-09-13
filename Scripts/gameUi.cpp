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
	LoadDivGraph("Resources/Textures/alhaDigital.png", 10,
		4, 4,
		64, 64, numGraph);

	//�����N
	LoadDivGraph("Resources/Textures/rank.png", MaxRank,
		4, 2,
		64, 64, rankGraph);
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
	DrawUI(timerT1, 1070, 100);
	//10�̈�
	DrawUI(timerT10, 1020, 100);
	//100�̈�
	DrawUI(timerT100, 970, 100);
	//1000�̈�
	DrawUI(timerT1000, 920, 100);

	DrawString(950, 30, "TIME", GetColor(0, 0, 0));
	DrawFormatString(1150, 155, GetColor(0, 0, 0), "%d", autotimer);
	
	//�c��[�w��m����UI
	//1�̈�
	DrawUI(depthT1, 1070, 330);
	//10�̈�
	DrawUI(depthT10, 1020, 330);
	//100�̈�
	DrawUI(depthT100, 970, 330);
	//1000�̈�
	DrawUI(depthT1000, 920, 330);

	DrawString(950, 260, "DEPTH", GetColor(0, 0, 0));
	DrawString(1150, 385, "m", GetColor(0, 0, 0));
	//�[�w�l��9999���J���X�g
	if (depthT1 >= 9999)
	{
		DrawString(1150, 350, "+", GetColor(0, 0, 0));
	}

	//�u���b�N�������󂵂����̂���UI
	//1�̈�
	DrawUI(blockCountT1, 1070, 590);
	//10�̈�
	DrawUI(blockCountT10, 1020, 590);
	//100�̈�
	DrawUI(blockCountT100, 970, 590);
	//1000�̈�
	DrawUI(blockCountT1000, 920, 590);

	DrawString(950, 500, "�󂵂��u���b�N��", GetColor(0, 0, 0));
	DrawString(1140, 640, "��", GetColor(0, 0, 0));
	//�u���b�N�j��J�E���g��9999���J���X�g
	if (blockCountT1 >= 9999)
	{
		DrawString(1150, 590, "+", GetColor(0, 0, 0));
	}
}

void gameUi::DrawUI(int digTimerRank, int uiPosX, int uiPosY)
{
	DrawGraph(uiPosX, uiPosY, numGraph[digTimerRank % 10], true);
}

void gameUi::DrawRank()
{

	if (rankTimer>=5){
		//depth�̕]���ꗗ
		if (depthT1 >= 0 && depthT1 <=20) {DrawGraph(0, 0, rankGraph[D], true);}
		else if (depthT1 >= 21 && depthT1 <= 60) {DrawGraph(0, 0, rankGraph[C], true);}
		else if (depthT1 >= 61 && depthT1 <= 100) {DrawGraph(0, 0, rankGraph[B], true);}
		else if (depthT1 >= 101 && depthT1 <= 140) {DrawGraph(0, 0, rankGraph[A], true);}
		else if (depthT1 >= 141) {DrawGraph(0, 0, rankGraph[S], true);}

	}
	if (rankTimer >= 10){
		//�󂵂��u���b�N�̕]���ꗗ
		if (blockCountT1>=0 && blockCountT1 <= 100) {DrawGraph(0, 0, rankGraph[D], true);}
		else if (blockCountT1 >= 101 && blockCountT1 <= 200) {DrawGraph(0, 0, rankGraph[C], true);}
		else if (blockCountT1 >= 201 && blockCountT1 <= 300) {DrawGraph(0, 0, rankGraph[B], true);}
		else if (blockCountT1 >= 301 && blockCountT1 <= 500) {DrawGraph(0, 0, rankGraph[A], true);}
		else if (blockCountT1 >= 501) {DrawGraph(0, 0, rankGraph[S], true);}

	}
	if (rankTimer >= 15)
	{
		//�ŏI�̍��Z�X�R�A�̕]���ꗗ(�����傫��)
		scoreRank currentRank = D;
		if (depthT1 + blockCountT1 >= 0 && depthT1 + blockCountT1 <= 120) { currentRank = D; }
		else if (depthT1 + blockCountT1 >= 121 && depthT1 + blockCountT1 <= 260) { currentRank = C; }
		else if (depthT1 + blockCountT1 >= 261 && depthT1 + blockCountT1 <= 400) { currentRank = B; }
		else if (depthT1 + blockCountT1 >= 401 && depthT1 + blockCountT1 <= 640) { currentRank = A; }
		else if (depthT1 + blockCountT1 >= 641) { currentRank = S; }

		DrawRotaGraph(500, 300, 4, 0, rankGraph[currentRank], true);
	}
	if (rankTimer >= 30)
	{
		isrankEnd = true;
	}
	rankTimer++;
}
