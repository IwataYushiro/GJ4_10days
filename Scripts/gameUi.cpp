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

	//depth�̃X�R�A
	scoredepGraph[S] = LoadGraph("Resources/Textures/scoredepS.png");
	scoredepGraph[A] = LoadGraph("Resources/Textures/scoredepA.png");
	scoredepGraph[B] = LoadGraph("Resources/Textures/scoredepB.png");
	scoredepGraph[C] = LoadGraph("Resources/Textures/scoredepC.png");
	scoredepGraph[D] = LoadGraph("Resources/Textures/scoredepD.png");
	//�󂵂��u���b�N�̃X�R�A
	scorebloGraph[S] = LoadGraph("Resources/Textures/scorebloS.png");
	scorebloGraph[A] = LoadGraph("Resources/Textures/scorebloA.png");
	scorebloGraph[B] = LoadGraph("Resources/Textures/scorebloB.png");
	scorebloGraph[C] = LoadGraph("Resources/Textures/scorebloC.png");
	scorebloGraph[D] = LoadGraph("Resources/Textures/scorebloD.png");
	//2�����킹���ŏI�X�R�A
	scorelastGraph[S] = LoadGraph("Resources/Textures/scorelastS.png");
	scorelastGraph[A] = LoadGraph("Resources/Textures/scorelastA.png");
	scorelastGraph[B] = LoadGraph("Resources/Textures/scorelastB.png");
	scorelastGraph[C] = LoadGraph("Resources/Textures/scorelastC.png");
	scorelastGraph[D] = LoadGraph("Resources/Textures/scorelastD.png");
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
	if (depthT1 >= 9999)
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
		if (depthT1 >= 0 && depthT1 <=20) {DrawGraph(0, 0, scoredepGraph[D], true);}
		else if (depthT1 >= 21 && depthT1 <= 60) {DrawGraph(0, 0, scoredepGraph[C], true);}
		else if (depthT1 >= 61 && depthT1 <= 100) {DrawGraph(0, 0, scoredepGraph[B], true);}
		else if (depthT1 >= 101 && depthT1 <= 140) {DrawGraph(0, 0, scoredepGraph[A], true);}
		else if (depthT1 >= 141) {DrawGraph(0, 0, scoredepGraph[S], true);}

	}
	if (rankTimer >= 10){
		//�󂵂��u���b�N�̕]���ꗗ
		if (blockCountT1>=0 && blockCountT1 <= 100) {DrawGraph(0, 0, scorebloGraph[D], true);}
		else if (blockCountT1 >= 101 && blockCountT1 <= 200) {DrawGraph(0, 0, scorebloGraph[C], true);}
		else if (blockCountT1 >= 201 && blockCountT1 <= 300) {DrawGraph(0, 0, scorebloGraph[B], true);}
		else if (blockCountT1 >= 301 && blockCountT1 <= 500) {DrawGraph(0, 0, scorebloGraph[A], true);}
		else if (blockCountT1 >= 501) {DrawGraph(0, 0, scorebloGraph[S], true);}

	}
	if (rankTimer >= 15)
	{
		//�ŏI�̍��Z�X�R�A�̕]���ꗗ
		if ( depthT1 + blockCountT1 >= 0 && depthT1 + blockCountT1 <= 120) {DrawGraph(0, 0, scorelastGraph[D], true);}
		else if (depthT1 + blockCountT1 >= 121 && depthT1 + blockCountT1 <= 260) {DrawGraph(0, 0, scorelastGraph[C], true);}
		else if (depthT1 + blockCountT1 >= 261 && depthT1 + blockCountT1 <= 400) {DrawGraph(0, 0, scorelastGraph[B], true);}
		else if (depthT1 + blockCountT1 >= 401 && depthT1 + blockCountT1 <= 640) {DrawGraph(0, 0, scorelastGraph[A], true);}
		else if (depthT1 + blockCountT1 >= 641) {DrawGraph(0, 0, scorelastGraph[S], true);}

	}
	if (rankTimer >= 30)
	{
		isrankEnd = true;
	}
	rankTimer++;
}
