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
	LoadDivGraph("Resources/Textures/alhaDigital.png", 10,
		4, 4,
		64, 64, numGraph);

	//depthのスコア
	scoredepGraph[S] = LoadGraph("Resources/Textures/scoredepS.png");
	scoredepGraph[A] = LoadGraph("Resources/Textures/scoredepA.png");
	scoredepGraph[B] = LoadGraph("Resources/Textures/scoredepB.png");
	scoredepGraph[C] = LoadGraph("Resources/Textures/scoredepC.png");
	scoredepGraph[D] = LoadGraph("Resources/Textures/scoredepD.png");
	//壊したブロックのスコア
	scorebloGraph[S] = LoadGraph("Resources/Textures/scorebloS.png");
	scorebloGraph[A] = LoadGraph("Resources/Textures/scorebloA.png");
	scorebloGraph[B] = LoadGraph("Resources/Textures/scorebloB.png");
	scorebloGraph[C] = LoadGraph("Resources/Textures/scorebloC.png");
	scorebloGraph[D] = LoadGraph("Resources/Textures/scorebloD.png");
	//2つを合わせた最終スコア
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
	if (depthT1 >= 9999)
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
	DrawString(1140, 640, "個", GetColor(0, 0, 0));
	//ブロック破壊カウントも9999がカンスト
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
		//depthの評価一覧
		if (depthT1 >= 0 && depthT1 <=20) {DrawGraph(0, 0, scoredepGraph[D], true);}
		else if (depthT1 >= 21 && depthT1 <= 60) {DrawGraph(0, 0, scoredepGraph[C], true);}
		else if (depthT1 >= 61 && depthT1 <= 100) {DrawGraph(0, 0, scoredepGraph[B], true);}
		else if (depthT1 >= 101 && depthT1 <= 140) {DrawGraph(0, 0, scoredepGraph[A], true);}
		else if (depthT1 >= 141) {DrawGraph(0, 0, scoredepGraph[S], true);}

	}
	if (rankTimer >= 10){
		//壊したブロックの評価一覧
		if (blockCountT1>=0 && blockCountT1 <= 100) {DrawGraph(0, 0, scorebloGraph[D], true);}
		else if (blockCountT1 >= 101 && blockCountT1 <= 200) {DrawGraph(0, 0, scorebloGraph[C], true);}
		else if (blockCountT1 >= 201 && blockCountT1 <= 300) {DrawGraph(0, 0, scorebloGraph[B], true);}
		else if (blockCountT1 >= 301 && blockCountT1 <= 500) {DrawGraph(0, 0, scorebloGraph[A], true);}
		else if (blockCountT1 >= 501) {DrawGraph(0, 0, scorebloGraph[S], true);}

	}
	if (rankTimer >= 15)
	{
		//最終の合算スコアの評価一覧
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
