#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>

class gameUi
{
public:

	void Reset();

	void Initialize();

	void Update();

	void Draw();

	//////////////////////////
	////�c�莞�Ԃ������^�C�}�[////
	/////////////////////////
	int zeroGraph;
	int oneGraph;
	int twoGraph;
	int threeGraph;
	int fourGraph;
	int fiveGraph;
	int sixGraph;
	int sevenGraph;
	int eightGraph;
	int nineGraph;

	//��ɓ����Ă�^�C�}�[
	int autotimer = 0;
	//1�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT1 = 0;
	//10�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT2 = 0;
	//100�̈ʂ̃^�C�}�[(autotimer��60�J�E���g�ɂȂ邲�Ƃ�1�ϓ�)
	int timerT3 = 0;
	//�����܂�
	////////////////////////////////////////////////////

	/////////////////////////////////////////////
	////////�[�w��m�܂ł�����ł��邩?��UI////////////
	////////////////////////////////////////////
	// 	//��ɓ����Ă�^�C�}�[
	//1�̈�
	int digTimerT1 = 0;
	//10�̈�
	int digTimerT2 = 0;

};