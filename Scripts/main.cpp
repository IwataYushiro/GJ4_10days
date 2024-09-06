#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "GJ4_Gamejam";

//�u���b�N�̑傫��
const int BLOCK_RADIUS = 64;
//����̐�
const int PLAYPART_WIDTH = 14;
//1���̏c�̒���
const int PLAYPART_HEIGHT = 100;
//UI���C��
const int GAME_LINE = BLOCK_RADIUS * PLAYPART_WIDTH;

// �E�B���h�E����
const int WIN_WIDTH = 1280;
// �E�B���h�E�c��
const int WIN_HEIGHT = 720;

//�t�H���g�̃T�C�Y
const int FONT_SIZE = 24;

enum Scene
{
	logo,
	title,
	playpart,
	credit,
	howtoplay,
};

enum BlockType
{
	weakblock,
	sandblock,
	frameblock,
	untappableblock,
	lethalblock,
};

struct MouseInputData {
	Vector2D position = { 0, 0 };
	Vector2D pin = { 0, 0 };
	bool click = false;
	bool preClick = false;
};

struct Button {
	Rect entity;
	string text;
	string eText;
	bool toggleEnabled;
	bool toggleStatus;
	int status;
};

struct Block
{
	RigidBody rigidBody;
	BlockType blockType;
	bool breaked = false;
};

struct LiveEntity
{
	RigidBody rigidBody;
	bool direction;
	int stuckFrameCount;
};

Vector2D GetMousePositionToV2D()
{
	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);
	return Vector2D{ (double)mouseX, (double)mouseY };
}

bool GetLeftClick()
{
	return GetMouseInput() & MOUSE_INPUT_LEFT;
}

void MouseInputDataUpdate(MouseInputData* mouseInputData)
{
	mouseInputData->preClick = mouseInputData->click;
	mouseInputData->click = GetLeftClick();

	mouseInputData->position = GetMousePositionToV2D();
	if (!mouseInputData->click)
	{
		mouseInputData->pin = mouseInputData->position;
	}
}

void ButtonUpdate(Button& button, MouseInputData mouseInputData, int pushSound)
{
	bool isHitMousePoint = HitRectAndPoint(button.entity, mouseInputData.position);
	if (!isHitMousePoint)
	{
		button.status = 0;
		return;
	}
	else
	{
		if (mouseInputData.click) {
			if (isHitMousePoint)
			{
				button.status = 2;
				return;
			}
		}
		else
		{
			if (mouseInputData.preClick && isHitMousePoint)
			{
				PlaySoundMem(pushSound, DX_PLAYTYPE_BACK, true);
				button.status = 3;
				return;
			}
			else
			{
				button.status = 1;
			}
		}
	}
}

void DrawButton(Button button)
{
	unsigned int color[3] = { 70, 70, 70 };
	switch (button.status) {
	case 0:
		color[0] = 255;
		color[1] = 255;
		color[2] = 255;
		break;
	case 1:
		color[0] = 255;
		color[1] = 180;
		color[2] = 255;
		break;
	case 2:
		color[0] = 0;
		color[1] = 0;
		color[2] = 0;
		break;
	case 3:
		color[0] = 255;
		color[1] = 0;
		color[2] = 0;
		break;
	}
	DrawBox(
		(int)(button.entity.x + button.entity.w),
		(int)(button.entity.y + button.entity.h),
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y - button.entity.h),
		GetColor(color[0], color[1], color[2]), TRUE);
	DrawLine(
		(int)(button.entity.x + button.entity.w - 1),
		(int)(button.entity.y - button.entity.h),
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y - button.entity.h),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawLine(
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y + button.entity.h - 1),
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y - button.entity.h - 1),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawTriangle(
		(int)(button.entity.x - button.entity.w),
		(int)(button.entity.y + button.entity.h),
		(int)(button.entity.x + button.entity.w),
		(int)(button.entity.y - button.entity.h),
		(int)(button.entity.x + button.entity.w),
		(int)(button.entity.y + button.entity.h),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2), TRUE);
	if (button.toggleEnabled) {
		DrawBox(
			(int)(button.entity.x + button.entity.w),
			(int)(button.entity.y - button.entity.h / 2),
			(int)(button.entity.x + button.entity.w / 2),
			(int)(button.entity.y - button.entity.h), GetColor(100, 100, 100), TRUE);
		if (button.toggleStatus) {
			DrawBox(
				(int)(button.entity.x + button.entity.w / 4 * 3),
				(int)(button.entity.y - button.entity.h / 2),
				(int)(button.entity.x + button.entity.w / 2),
				(int)(button.entity.y - button.entity.h), GetColor(100, 255, 100), TRUE);
		}
		else {
			DrawBox(
				(int)(button.entity.x + button.entity.w / 4 * 3),
				(int)(button.entity.y - button.entity.h / 2),
				(int)(button.entity.x + button.entity.w),
				(int)(button.entity.y - button.entity.h), GetColor(150, 0, 0), TRUE);
		}
	}
	DrawString(
		button.entity.x - button.entity.w, button.entity.y - button.entity.h,
		button.eText.c_str(),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawString(
		button.entity.x, button.entity.y, button.text.c_str(),
		GetColor(0, 0, 0));
}

bool IsButtonClicked(vector<Button>& buttons, int buttonIndex)
{
	return buttons.size() > buttonIndex && buttons[buttonIndex].status == 3;
}

void LiveEntityUpdate(LiveEntity* liveEntity, std::vector<GameObject> blocks)
{
	//��������
	RigidBodyUpdate(liveEntity->rigidBody, { 0,1 }, { 0.5,1 }, blocks);
	//���n���Ă�����
	if (liveEntity->rigidBody.landing)
	{
		//���΂炭�O���ɐi�߂Ȃ���Δ��]
		if ((liveEntity->direction
			&& liveEntity->rigidBody.gameObject.beforePos.x >= liveEntity->rigidBody.gameObject.entity.x)
			|| (!liveEntity->direction
				&& liveEntity->rigidBody.gameObject.beforePos.x <= liveEntity->rigidBody.gameObject.entity.x))
		{
			liveEntity->stuckFrameCount++;
		}
		else
		{
			liveEntity->stuckFrameCount--;
		}
		liveEntity->stuckFrameCount = min(max(0, liveEntity->stuckFrameCount), 3);

		if (liveEntity->stuckFrameCount >= 3)
		{
			liveEntity->direction = !liveEntity->direction;
			liveEntity->stuckFrameCount = 0;
		}

		//�O�i
		float playerMoveForce = 5;
		if (!liveEntity->direction)
		{
			playerMoveForce *= -1;
		}
		liveEntity->rigidBody.movement.x += playerMoveForce;
	}
	else
	{
		liveEntity->stuckFrameCount = 0;
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {

	// �E�B���h�E���[�h�ɐݒ�
	ChangeWindowMode(TRUE);

	// �E�B���h�E�T�C�Y���蓮�ł͕ύX�������A
	// ���E�B���h�E�T�C�Y�ɍ��킹�Ċg��ł��Ȃ��悤�ɂ���
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// �^�C�g����ύX
	SetMainWindowText(TITLE);

	// ��ʃT�C�Y�̍ő�T�C�Y�A�J���[�r�b�g����ݒ�(���j�^�[�̉𑜓x�ɍ��킹��)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// ��ʃT�C�Y��ݒ�(�𑜓x�Ƃ̔䗦�Őݒ�)
	SetWindowSizeExtendRate(1.0);

	// ��ʂ̔w�i�F��ݒ肷��
	SetBackgroundColor(0xff, 0xee, 0xaa);

	// DXlib�̏�����
	if (DxLib_Init() == -1) { return -1; }

	// (�_�u���o�b�t�@)�`���O���t�B�b�N�̈�͗��ʂ��w��
	SetDrawScreen(DX_SCREEN_BACK);

	// �t�H���g�̃T�C�Y��ݒ肷��
	SetFontSize(FONT_SIZE);

	// �摜�Ȃǂ̃��\�[�X�f�[�^�̕ϐ��錾�Ɠǂݍ���

	//�J���҃��S
	const int logoGraph = LoadGraph("Resources/Textures/TERAPETAGAMES_logo.png");
	//�^�C�g�����(�^�C�g����ʂ���V���[�e�B���O�Q�[��)��BGM
	const int titleGraph = LoadGraph("Resources/Textures/title.png");
	const int operationGraph = LoadGraph("Resources/Textures/sousa.png");
	//�N���A���(�V���[�e�B���O�Q�[������N���A���)��BGM
	const int clearGraph = LoadGraph("Resources/Textures/clear.png");
	//���@
	const int playerSprite = LoadGraph("Resources/Textures/frameBlock.png");
	//�u���b�N�e��
	const int blocksSprite[] = {
		LoadGraph("Resources/Textures/weakBlock.png"),
		LoadGraph("Resources/Textures/sandBlock.png"),
		LoadGraph("Resources/Textures/frameBlock.png"),
		LoadGraph("Resources/Textures/unTappableBlock.png"),
		LoadGraph("Resources/Textures/lethalBlock.png"),
	};

	//�r�b�g�}�b�v�t�H���g�p�̐����摜�ƃ^�C�}�[
	const int zeroGraph = LoadGraph("Resources/Textures/timer0.png");
	const int oneGraph = LoadGraph("Resources/Textures/timer1.png");
	const int twoGraph = LoadGraph("Resources/Textures/timer2.png");
	const int threeGraph = LoadGraph("Resources/Textures/timer3.png");
	const int fourGraph = LoadGraph("Resources/Textures/timer4.png");
	const int fiveGraph = LoadGraph("Resources/Textures/timer5.png");
	const int sixGraph = LoadGraph("Resources/Textures/timer6.png");
	const int sevenGraph = LoadGraph("Resources/Textures/timer7.png");
	const int eightGraph = LoadGraph("Resources/Textures/timer8.png");
	const int nineGraph = LoadGraph("Resources/Textures/timer9.png");
	int timer = 0;
	//1�̈ʂ̃^�C�}�[
	int timerT1 = 0;
	//10�̈ʂ̃^�C�}�[
	int timer2 = 0;
	//100�̈ʂ̃^�C�}�[
	int timer3 = 0;


	//�{�^����������
	const int buttonPushSound = LoadSoundMem("Resources/SE/buttonPush.wav");

	// �Q�[�����[�v�Ŏg���ϐ��̐錾

	//�}�E�X���͂������t�B�[���h
	MouseInputData mouseInputData;

	//���݂̃V�[��
	Scene currentScene = logo;
	//�J�ڂ��悤�Ƃ��Ă��鎟�̃V�[��
	Scene nextScene = logo;

	//�V�[���J�ڗp�̃^�C�}�[
	float sceneTransitionProgress = 0;
	//Pause���̃t���O
	bool isPause = false;
	//�{�^��
	vector<Button> buttons;

	//�J�������W
	Vector2D camPosition = Vector2D{ 0,0 };
	const Vector2D camPosOffset = Vector2D{ -WIN_WIDTH / 2,-WIN_HEIGHT / 2 };


	//���@
	LiveEntity player = LiveEntity{ RigidBody{ GameObject{ Rect{0,0,64,64}, playerSprite} } };
	//���@���t�B�[���h�O�ɏo�Ȃ����߂̕�
	vector<GameObject> edgeWall = {
		GameObject{Rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{Rect{-WIN_WIDTH / 2 + GAME_LINE,0,0,WIN_HEIGHT}},
		GameObject{Rect{0,-WIN_HEIGHT / 2,WIN_WIDTH,0}},
		GameObject{Rect{0,WIN_HEIGHT / 2,WIN_WIDTH,0}},
	};
	//�u���b�N
	vector<Block> blocks = {};


	// �ŐV�̃L�[�{�[�h���p
	char keys[256] = { 0 };

	// 1���[�v(�t���[��)�O�̃L�[�{�[�h���
	char oldkeys[256] = { 0 };

	// �Q�[�����[�v
	while (true) {
		// �ŐV�̃L�[�{�[�h��񂾂������̂�1�t���[���O�̃L�[�{�[�h���Ƃ��ĕۑ�
		// �ŐV�̃L�[�{�[�h�����擾
		GetHitKeyStateAll(keys);
		// ��ʃN���A
		ClearDrawScreen();
		//---------  ��������v���O�������L�q  ----------//

		// �X�V����

		//�}�E�X���X�V
		MouseInputDataUpdate(&mouseInputData);

		//�{�^���z������Z�b�g
		buttons = {};

		bool sceneInit = false;
		if (nextScene == currentScene)
		{
			sceneTransitionProgress = 0;
		}
		else
		{
			//�V�[����؂�ւ��̃J�E���g�_�E��
			sceneTransitionProgress++;
			if (sceneTransitionProgress >= 30) {
				//�V�[����؂�ւ��A�������t���O�𗧂Ă�
				currentScene = nextScene;
				sceneInit = true;
			}
		}

		//�V�[������������
		if (sceneInit)
		{
			switch (currentScene)
			{
			case title:
				//�^�C�g�����
				break;
			case playpart:
				//�v���C�p�[�g

				//�|�[�Y����
				isPause = false;
				//���@���������W��
				player = LiveEntity{ RigidBody{ GameObject{ Rect{-WIN_WIDTH / 2 + GAME_LINE / 2,-WIN_HEIGHT / 2 + 32,50,64}, playerSprite} } };
				//�u���b�N���������A����
				blocks = {};
				for (int i = 0; i < PLAYPART_HEIGHT; i++)
				{
					for (int j = 0; j < PLAYPART_WIDTH; j++)
					{
						BlockType currentBlockType = (BlockType)(rand() % 5);
						if (i == 0)
						{
							currentBlockType = weakblock;
						}

						blocks.push_back(Block{ RigidBody{GameObject{
							Rect{(float)-WIN_WIDTH / 2 + BLOCK_RADIUS / 2 + BLOCK_RADIUS * j,(float)BLOCK_RADIUS * i,BLOCK_RADIUS,BLOCK_RADIUS}
						}},currentBlockType });
					}
				}
				break;
			}
		}

		//�{�^���𐶐�
		switch (currentScene)
		{
		case title:
			//�^�C�g�����

			//�X�^�[�g�{�^���ƃN���W�b�g�{�^��
			buttons = {
				Button{Rect{WIN_WIDTH / 2, WIN_HEIGHT / 4 * 3,200,100},"�X�^�[�g","START"},
				Button{Rect{140, 60,130,50},"�N���W�b�g","CREDITS"},
				Button{Rect{WIN_WIDTH - 140, 60,130,50},"�V�ѕ�","HOW TO PLAY"} ,
			};
			break;
		case playpart:
			//�v���C�p�[�g

			if (isPause)
			{
				//�|�[�Y���j���[�{�^��
				buttons = {
					Button{Rect{370, 160,60,50},"�Â���","RESUME"},
					Button{Rect{370, 460,60,50},"��߂�","QUIT"},
				};
			}
			else
			{
				//�|�[�Y�{�^��
				buttons = {
					Button{Rect{70, 60,60,50},"�߰��","PAUSE"},
				};
			}
			break;
		case credit:
			//�N���W�b�g���

			//�^�C�g���ɖ߂�{�^��
			buttons = {
				Button{Rect{140, 60,130,50},"���ǂ�","RETURN"},
			};
			break;
		case howtoplay:
			//�V�ѕ��������

			//�^�C�g���ɖ߂�{�^��
			buttons = {
				Button{Rect{140, 60,130,50},"���ǂ�","RETURN"},
			};
			break;
		}

		//�{�^�����X�V�i������Ƃ����c�ɗh�炷�j
		for (int i = 0; i < buttons.size(); i++) {
			buttons[i].entity.y += sin(clock() / PI / 300 - i / 2.0) * 5;
			ButtonUpdate(buttons[i], mouseInputData, buttonPushSound);
		}

		switch (currentScene)
		{
		case logo:
			//���S���

			//���N���b�N�Ń^�C�g����
			if (!mouseInputData.click && mouseInputData.preClick)
			{
				nextScene = title;
			}
			break;
		case title:
			//�^�C�g�����

			//�{�^�������������̏���
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = playpart;
			}
			if (IsButtonClicked(buttons, 1))
			{
				nextScene = credit;
			}
			if (IsButtonClicked(buttons, 2))
			{
				nextScene = howtoplay;
			}
			break;
		case playpart:
			//�v���C�p�[�g
			if (!isPause)
			{
				timer++;
				if (timer >= 58) {
					timerT1++;
					timer = 0;
				}
				if (timerT1 >= 10) {
					timerT1 = 0;
				}


				//�S�Ẵu���b�N���X�V
				for (int i = 0; i < blocks.size(); i++)
				{
					blocks[i].rigidBody.gameObject.graphNum = blocksSprite[blocks[i].blockType];
					//�N���b�N���ꂽ����������i�j��\�ȃu���b�N�̂݁j
					if (blocks[i].blockType != untappableblock && blocks[i].blockType != lethalblock
						&& HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.position + (camPosition + camPosOffset))
						&& HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.pin + (camPosition + camPosOffset))
						&& !mouseInputData.click && mouseInputData.preClick)
					{
						blocks[i].breaked = true;
					}
				}
				//�����������o�����u���b�N��S������
				for (int i = 0; i < blocks.size(); i++)
				{
					if (blocks[i].breaked)
					{
						blocks.erase(blocks.begin() + i);
						i--;
					}
				}

				//�l���̕ǂƃu���b�N�i�t���[���u���b�N�ȊO�j��ǂƂ���
				vector<GameObject> liveEntityWalls = edgeWall;
				for (int i = 0; i < blocks.size(); i++)
				{
					if (blocks[i].blockType != frameblock)
					{
						liveEntityWalls.push_back(blocks[i].rigidBody.gameObject);
					}
				}
				//���@���X�V
				LiveEntityUpdate(&player, liveEntityWalls);

				//�J�����Ǐ]
				camPosition = Vector2D{ 0,player.rigidBody.gameObject.entity.y };

				//�{�^�������������̏���
				if (IsButtonClicked(buttons, 0))
				{
					//�|�[�Y����
					isPause = true;
				}
			}
			else
			{
				//�|�[�Y��

				//������
				if (IsButtonClicked(buttons, 0))
				{
					//�|�[�Y����
					isPause = false;
				}
				//�I���
				if (IsButtonClicked(buttons, 1))
				{
					//�^�C�g����
					nextScene = title;
				}
			}


			break;
		case credit:
			//�N���W�b�g���

			//�{�^�������������̏���
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		case howtoplay:
			//�V�ѕ��������

			//�{�^�������������̏���
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		}

		// �`�揈��
		switch (currentScene)
		{
		case logo:
			//���S���
			DrawString(
				WIN_WIDTH / 7 * 3, WIN_HEIGHT / 4,
				"Developed by",
				GetColor(0, 0, 0));
			DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 2, 0, logoGraph, TRUE);
			DrawString(
				WIN_WIDTH / 7 * 3, WIN_HEIGHT / 5 * 3,
				"TEAM GJ4",
				GetColor(0, 0, 0));
			if (clock() % 1500 < 1000)
			{
				DrawString(
					WIN_WIDTH / 3, WIN_HEIGHT - FONT_SIZE * 4,
					"��ʂ����N���b�N���Ă�������",
					GetColor(0, 0, 0));
			}
			break;
		case title:
			//�^�C�g�����

			//�^�C�g�����S
			DrawGraph(0, 0, titleGraph, true);
			DrawGraph(0, 0, operationGraph, true);
			//�����\��
			DrawString(
				WIN_WIDTH / 3, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "2024 TERAPETA GAMES / TEAM GJ4",
				GetColor(0, 0, 0));
			break;
		case playpart:
			//�v���C�p�[�g

			//�S�Ẵu���b�N��`��
			for (int i = 0; i < blocks.size(); i++) {
				RenderObject(blocks[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//���@��`��
			RenderObject(player.rigidBody.gameObject, camPosition + camPosOffset);
			//���̃��C�������UI�]�[���Ȃ̂ł����������Ƀ{�b�N�XUI�����Ă�������
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);

			//�r�b�g�}�b�v�t�H���g�^�C�}�[
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

			DrawFormatString(800, 300, GetColor(122, 112, 122), "%d", timer);

			break;
		case credit:
			//�N���W�b�g���
			DrawString(
				WIN_WIDTH / 4, 0,
				"\n\n�v���O���}�[\n�@�񃆃E\n�@�_����\n\n�`�[�t�v���O���}�[\n�@�Ă�؂�\n\n�A�[�g\n�@�Ă�؂�\n\n�G�t�F�N�g\n�@�Ă�؂�\n\n�T�E���h�G�t�F�N�g\n�@�Ă�؂�\n\n�R���|�[�T�[\n�@�Ă�؂�",
				GetColor(0, 0, 0));
			DrawString(
				WIN_WIDTH / 2, 0,
				"\n\n�r�W���A���A�h�o�C�U�[\n�@�Ă�؂�\n\n�G�O�[�N�e�B�u�v���f���[�T�[\n�@�Ă�؂�\n\n�f�B���N�^�[\n�@�Ă�؂�\n\n�����͂�\n�@�Ă�؂��Q�[���Y\n�@�`�[��GJ4\n\n\nTERAPETA GAMES / TEAM GJ4\nAll Rights Reserved.",
				GetColor(0, 0, 0));
			break;
		case howtoplay:
			DrawGraph(0, 0, operationGraph, true);
			break;
		}

		//�S�Ẵ{�^����`��
		for (int i = 0; i < buttons.size(); i++)
		{
			DrawButton(buttons[i]);
		}

		//---------  �����܂łɃv���O�������L�q  ---------//
		// (�_�u���o�b�t�@)����
		ScreenFlip();

		// 20�~���b�ҋ@(�^��60FPS)
		WaitTimer(20);

		// Windows�V�X�e�����炭�������������
		if (ProcessMessage() == -1) {
			break;
		}

		// ESC�L�[�������ꂽ�烋�[�v���甲����
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	// Dx���C�u�����I������
	DxLib_End();

	// ����I��
	return 0;
}
