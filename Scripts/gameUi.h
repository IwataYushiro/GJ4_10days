#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>

enum scoreRank
{
	S,
	A,
	B,
	C,
	D,
	MaxRank
};

class gameUi
{
public:

	void Reset();

	void Initialize();

	void Update();

	void Draw();

	//��ɓ����Ă�^�C�}�[
	int autotimer = 0;
	//1�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT1 = 0;
	//10�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT10 = 0;
	//100�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT100 = 0;
	//100�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT1000 = 0;
	//�����܂�
	////////////////////////////////////////////////////

	/////////////////////////////////////////////
	////////�[�w��m�܂ł�����ł��邩?��UI////////////
	////////////////////////////////////////////
	// 	//��ɓ����Ă�^�C�}�[
	//1�̈�
	int depthT1 = 0;
	//10�̈�
	int depthT10 = 0;
	//100�̈�
	int depthT100 = 0;
	//1000�̈�
	int depthT1000 = 0;
	//�����܂�
	////////////////////////////////////////////////////

	/////////////////////////////////////////////
	//////���u���b�N���󂵂Ă��邩�H��UI////////////
	////////////////////////////////////////////
	//��ɓ����Ă�^�C�}�[
	//1�̈�
	int blockCountT1 = 0;
	//10�̈�
	int blockCountT10 = 0;
	//100�̈�
	int blockCountT100 = 0;
	//1000�̈�
	int blockCountT1000 = 0;
	//�����܂�
	////////////////////////////////////////////////////

	//�����]���p�^�C�}�[
	int rankTimer = 0;
	//�����L���O���Z��{�^�����o�����߂̃t���O
	bool isrankEnd = false;
	
	//�����N�̕`��
	void DrawRank();
private:
	//�[�w�����[�g���܂Ői��ł邩��UI�`��(�����AX��)
	void DrawUI(int digTimerRank, int uiPosX, int uiposY);
	
private:
	///////////////////////////
	//�c�莞�Ԃ������^�C�}�[(�摜)//
	///////////////////////////
	int numGraph[10];

	//�X�R�A�\���p�̉摜
	//depth�̃X�R�A
	int scoredepGraph[MaxRank];
	//�󂵂��u���b�N�̃X�R�A
	int scorebloGraph[MaxRank];
	//2�����킹���ŏI�X�R�A
	int scorelastGraph[MaxRank];
};