#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "GJ4_Gamejam";

//�u���b�N�̑傫��
const int BLOCK_RADIUS = 128;
//����̐�
const int PLAYPART_WIDTH = 7;
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

struct MouseInputData {
	Vector2D position = { 0, 0 };
	Vector2D pin = { 0, 0 };
	bool click = false;
	bool preClick = false;
};

struct Button {
	rect entity;
	string text;
	string eText;
	bool toggleEnabled;
	bool toggleStatus;
	int status;
};

struct Player
{
	RigidBody rigidBody;
	bool direction;
	int stuckFrameCount;
};

enum Scene
{
	logo,
	title,
	playpart,
	credit,
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
	bool isHitMousePoint = HitRectAndPoint(button.entity, GetMousePositionToV2D());
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

void PlayerUpdate(Player* player, std::vector<GameObject> blocks)
{
	//��������
	RigidBodyUpdate(player->rigidBody, { 0,1 }, { 0.5,1 }, blocks);
	//���n���Ă�����
	if (player->rigidBody.landing)
	{
		//���΂炭�O���ɐi�߂Ȃ���Δ��]
		if ((player->direction
			&& player->rigidBody.gameObject.beforePos.x >= player->rigidBody.gameObject.entity.x)
			|| (!player->direction
				&& player->rigidBody.gameObject.beforePos.x <= player->rigidBody.gameObject.entity.x))
		{
			player->stuckFrameCount++;
		}
		else
		{
			player->stuckFrameCount--;
		}
		player->stuckFrameCount = min(max(0, player->stuckFrameCount), 3);

		if (player->stuckFrameCount >= 3)
		{
			player->direction = !player->direction;
			player->stuckFrameCount = 0;
		}

		//�O�i
		float playerMoveForce = 5;
		if (!player->direction)
		{
			playerMoveForce *= -1;
		}
		player->rigidBody.movement.x += playerMoveForce;
	}
	else
	{
		player->stuckFrameCount = 0;
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
	//�N���A���(�V���[�e�B���O�Q�[������N���A���)��BGM
	const int clearGraph = LoadGraph("Resources/Textures/clear.png");
	//���@
	const int playerSprite = LoadGraph("Resources/Textures/napnose.png");

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

	//�{�^��
	vector<Button> buttons;

	const Vector2D mapScale = { PLAYPART_WIDTH,PLAYPART_HEIGHT };

	Player player = Player{ RigidBody{ GameObject{ rect{0,0,64,64}, playerSprite} } };
	vector<GameObject> edgeWall = {
		GameObject{rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{rect{-WIN_WIDTH / 2 + GAME_LINE,0,0,WIN_HEIGHT}},
		GameObject{rect{0,-WIN_HEIGHT / 2,WIN_WIDTH,0}},
		GameObject{rect{0,WIN_HEIGHT / 2,WIN_WIDTH,0}},
	};
	vector<vector<int>> map = { {} };


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

		//�V�[���J�ړ��͂őΉ�����V�[���ւ̑J�ڂ�����
		if (!mouseInputData.click && mouseInputData.preClick)
		{
			switch (currentScene)
			{
			case logo:
				nextScene = title;
				break;
			}
		}

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

				//���@���������W��
				player = Player{ RigidBody{ GameObject{ rect{-WIN_WIDTH / 2 + GAME_LINE / 2,0,128,128}, playerSprite} } };
				break;
			default:
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
				Button{rect{WIN_WIDTH / 2, WIN_HEIGHT / 4 * 3,200,100},"�X�^�[�g\n","START\n"},
				Button{rect{140, 60,130,50},"�N���W�b�g\n","CREDITS\n"},
			};
			break;
		case playpart:
			//�v���C�p�[�g

			//�|�[�Y�{�^��
			buttons = {
				Button{rect{70, 60,60,50},"�߰��\n","PAUSE\n"},
			};
			break;
		case credit:
			//�N���W�b�g���

			//�^�C�g���ɖ߂�{�^��
			buttons = {
				Button{rect{140, 60,130,50},"���ǂ�\n","RETURN\n"},
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
			break;
		case playpart:
			//�v���C�p�[�g

			//���@���X�V
			PlayerUpdate(&player, edgeWall);

			//�{�^�������������̏���
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
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
		default:
			break;
		}

		// �`�揈��
		switch (currentScene)
		{
		case title:
			//�^�C�g�����

			//�^�C�g�����S
			DrawGraph(0, 0, titleGraph, true);
			//�����\��
			DrawString(
				WIN_WIDTH / 7 * 3, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "2024 TERAPETA GAMES",
				GetColor(0, 0, 0));
			break;
		case playpart:
			//�v���C�p�[�g

			//���@��`��
			RenderObject(player.rigidBody.gameObject, Vector2D{ -WIN_WIDTH / 2,WIN_HEIGHT / 2 });
			//���̃��C�������UI�]�[���Ȃ̂ł����������Ƀ{�b�N�XUI�����Ă�������
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);
			break;
		case logo:
			//���S���
			DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 2, 0, logoGraph, 0);
			break;
		case credit:
			//�N���W�b�g���
			DrawString(
				WIN_WIDTH / 2, FONT_SIZE * 2,
				"�v���O���}�[\n�@�񃆃E\n�@�_����\n\n�`�[�t�v���O���}�[\n�@�Ă�؂�\n\n�f�B���N�^�[\n�@�Ă�؂�\n",
				GetColor(0, 0, 0));
			break;

		}

		//�S�Ẵ{�^����`��
		for (int i = 0; i < buttons.size(); i++) {
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
