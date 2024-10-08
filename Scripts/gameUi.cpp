#include "gameUi.h"

const int textColor = 0x000080;

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

	//ランク
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
	DrawUI(timerT1, 1070, 100);
	//10の位
	DrawUI(timerT10, 1020, 100);
	//100の位
	DrawUI(timerT100, 970, 100);
	//1000の位
	DrawUI(timerT1000, 920, 100);

	DrawString(950, 30, "TIME", textColor);
	DrawFormatString(1150, 155, textColor, "%d", autotimer);
	
	//残り深層何mかのUI
	//1の位
	DrawUI(depthT1, 1070, 330);
	//10の位
	DrawUI(depthT10, 1020, 330);
	//100の位
	DrawUI(depthT100, 970, 330);
	//1000の位
	DrawUI(depthT1000, 920, 330);

	DrawString(950, 260, "DEPTH", textColor);
	DrawString(1150, 385, "cm", textColor);
	//深層値は9999がカンスト
	if (depthT1 >= 9999)
	{
		DrawString(1150, 350, "+", textColor);
	}

	//ブロックをいくつ壊したかのかのUI
	//1の位
	DrawUI(blockCountT1, 1070, 590);
	//10の位
	DrawUI(blockCountT10, 1020, 590);
	//100の位
	DrawUI(blockCountT100, 970, 590);
	//1000の位
	DrawUI(blockCountT1000, 920, 590);

	DrawString(950, 500, "壊したブロック数", textColor);
	DrawString(1140, 640, "個", textColor);
	//ブロック破壊カウントも9999がカンスト
	if (blockCountT1 >= 9999)
	{
		DrawString(1150, 590, "+", textColor);
	}
}

void gameUi::DrawUI(int digTimerRank, int uiPosX, int uiPosY)
{
	DrawGraph(uiPosX, uiPosY, numGraph[digTimerRank % 10], true);
}

void gameUi::DrawRank()
{
	const int scoreDepthX = 1180;
	const int scoreDepthY = 350;
	const int scoreBlockX = 1180;
	const int scoreBlockY = 600;
	const int rankrate = 4;
	if (rankTimer>=5){
		//depthの評価一覧
		if (depthT1 >= 0 && depthT1 <=20) {DrawGraph(scoreDepthX, scoreDepthY, rankGraph[D], true);}
		else if (depthT1 >= 21 && depthT1 <= 60) {DrawGraph(scoreDepthX, scoreDepthY, rankGraph[C], true);}
		else if (depthT1 >= 61 && depthT1 <= 100) {DrawGraph(scoreDepthX, scoreDepthY, rankGraph[B], true);}
		else if (depthT1 >= 101 && depthT1 <= 140) {DrawGraph(scoreDepthX, scoreDepthY, rankGraph[A], true);}
		else if (depthT1 >= 141) {DrawGraph(scoreDepthX, scoreDepthY, rankGraph[S], true);}

	}
	if (rankTimer >= 10){
		//壊したブロックの評価一覧
		if (blockCountT1/ rankrate >=0 && blockCountT1/rankrate <= 100) {DrawGraph(scoreBlockX, scoreBlockY, rankGraph[D], true);}
		else if (blockCountT1/rankrate >= 101 && blockCountT1/rankrate <= 200) {DrawGraph(scoreBlockX, scoreBlockY, rankGraph[C], true);}
		else if (blockCountT1/rankrate >= 201 && blockCountT1/rankrate <= 300) {DrawGraph(scoreBlockX, scoreBlockY, rankGraph[B], true);}
		else if (blockCountT1/rankrate >= 301 && blockCountT1/rankrate <= 500) {DrawGraph(scoreBlockX, scoreBlockY, rankGraph[A], true);}
		else if (blockCountT1/rankrate >= 501) {DrawGraph(scoreBlockX, scoreBlockY, rankGraph[S], true);}

	}
	if (rankTimer >= 15)
	{
		//最終の合算スコアの評価一覧(他より大きく)
		scoreRank currentRank = D;
		if (depthT1 + blockCountT1/ rankrate >= 0 && depthT1 + blockCountT1/ rankrate <= 120) { currentRank = D; }
		else if (depthT1 + blockCountT1/rankrate >= 121 && depthT1 + blockCountT1/rankrate <= 260) { currentRank = C; }
		else if (depthT1 + blockCountT1/rankrate >= 261 && depthT1 + blockCountT1/rankrate <= 400) { currentRank = B; }
		else if (depthT1 + blockCountT1/rankrate >= 401 && depthT1 + blockCountT1/rankrate <= 640) { currentRank = A; }
		else if (depthT1 + blockCountT1/rankrate>= 641) { currentRank = S; }

		DrawRotaGraph(500, 300, 4, 0, rankGraph[currentRank], true);
	}
	if (rankTimer >= 30)
	{
		isrankEnd = true;
	}
	rankTimer++;
}
