#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>
#include "gameUi.h"

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "GJ4_Gamejam";

//�u���b�N�̑傫��
const int BLOCK_DIAMETER = 64;
//����̐�
const int PLAYPART_WIDTH = 14;
//1���̏c�̒���
const int PLAYPART_HEIGHT = 100;
//UI���C��
const int GAME_LINE = BLOCK_DIAMETER * PLAYPART_WIDTH;

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
	unsigned int landingBlockIndex = 0;
	string status = "";
};

struct LiveEntity
{
	RigidBody rigidBody;
	bool isLive = true;
	bool direction = true;
	int stuckFrameCount = 0;
};

Vector2D GetMousePositionToV2D()
{
	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);
	return Vector2D{ (float)mouseX, (float)mouseY };
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
		(int)(button.entity.position.x + button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y),
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y),
		GetColor(color[0], color[1], color[2]), TRUE);
	DrawLine(
		(int)(button.entity.position.x + button.entity.scale.x - 1),
		(int)(button.entity.position.y - button.entity.scale.y),
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawLine(
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y - 1),
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y - 1),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawTriangle(
		(int)(button.entity.position.x - button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y),
		(int)(button.entity.position.x + button.entity.scale.x),
		(int)(button.entity.position.y - button.entity.scale.y),
		(int)(button.entity.position.x + button.entity.scale.x),
		(int)(button.entity.position.y + button.entity.scale.y),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2), TRUE);
	if (button.toggleEnabled) {
		DrawBox(
			(int)(button.entity.position.x + button.entity.scale.x),
			(int)(button.entity.position.y - button.entity.scale.y / 2),
			(int)(button.entity.position.x + button.entity.scale.x / 2),
			(int)(button.entity.position.y - button.entity.scale.y), GetColor(100, 100, 100), TRUE);
		if (button.toggleStatus) {
			DrawBox(
				(int)(button.entity.position.x + button.entity.scale.x / 4 * 3),
				(int)(button.entity.position.y - button.entity.scale.y / 2),
				(int)(button.entity.position.x + button.entity.scale.x / 2),
				(int)(button.entity.position.y - button.entity.scale.y), GetColor(100, 255, 100), TRUE);
		}
		else {
			DrawBox(
				(int)(button.entity.position.x + button.entity.scale.x / 4 * 3),
				(int)(button.entity.position.y - button.entity.scale.y / 2),
				(int)(button.entity.position.x + button.entity.scale.x),
				(int)(button.entity.position.y - button.entity.scale.y), GetColor(150, 0, 0), TRUE);
		}
	}
	DrawString(
		button.entity.position.x - button.entity.scale.x, button.entity.position.y - button.entity.scale.y,
		button.eText.c_str(),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawString(
		button.entity.position.x, button.entity.position.y, button.text.c_str(),
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

	//�����Ă�����
	if (liveEntity->isLive)
	{
		//�����u���b�N�̒��ɉ������܂ꂽ�玀��
		for (int i = 0; i < blocks.size(); i++)
		{
			if (HitRectAndPoint(blocks[i].entity,
				Vector2D{ liveEntity->rigidBody.gameObject.entity.position.x,
				liveEntity->rigidBody.gameObject.entity.position.y }))
			{
				liveEntity->isLive = false;
			}
		}

		//���n���Ă�����
		if (liveEntity->rigidBody.landing)
		{
			//���΂炭�O���ɐi�߂Ȃ���Δ��]
			if ((liveEntity->direction
				&& liveEntity->rigidBody.gameObject.beforePos.x >= liveEntity->rigidBody.gameObject.entity.position.x)
				|| (!liveEntity->direction
					&& liveEntity->rigidBody.gameObject.beforePos.x <= liveEntity->rigidBody.gameObject.entity.position.x))
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
				liveEntity->rigidBody.gameObject.dir = !liveEntity->rigidBody.gameObject.dir;
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

	//�N���X�����w�b�_�[�ǂݍ���
	gameUi* gameui_ = nullptr;
	gameui_ = new gameUi();
	gameui_->Initialize();

	// �摜�Ȃǂ̃��\�[�X�f�[�^�̕ϐ��錾�Ɠǂݍ���

	//�J���҃��S
	const int logoGraph = LoadGraph("Resources/Textures/TERAPETAGAMES_logo.png");
	//�^�C�g�����(�^�C�g����ʂ���V���[�e�B���O�Q�[��)��BGM
	const int titleGraph = LoadGraph("Resources/Textures/title.png");
	const int operationGraph = LoadGraph("Resources/Textures/sousa.png");
	//�N���A���(�V���[�e�B���O�Q�[������N���A���)��BGM
	const int clearGraph = LoadGraph("Resources/Textures/clear.png");
	//���@
	const int playerSprite = LoadGraph("Resources/Textures/protoplayer.png");
	//�u���b�N�e��
	const int blocksSprite[] = {
		LoadGraph("Resources/Textures/weakBlock.png"),
		LoadGraph("Resources/Textures/sandBlock.png"),
		LoadGraph("Resources/Textures/frameBlock.png"),
		LoadGraph("Resources/Textures/unTappableBlock.png"),
		LoadGraph("Resources/Textures/lethalBlock.png"),
	};
	//�w�i
	const int backgroundSprite = LoadGraph("Resources/Textures/protobackground.png");

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
	LiveEntity player = {};
	//���@���t�B�[���h�O�ɏo�Ȃ����߂̕�
	vector<GameObject> edgeWall = {
		GameObject{Rect{-WIN_WIDTH / 2,0,0,WIN_HEIGHT}},
		GameObject{Rect{-WIN_WIDTH / 2 + GAME_LINE,0,0,WIN_HEIGHT}},
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
				player = LiveEntity{ RigidBody{ GameObject{ Rect{-WIN_WIDTH / 2 + GAME_LINE / 2,-WIN_HEIGHT / 2 + 32,25,25}, playerSprite} } };
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
							Rect{(float)-WIN_WIDTH / 2 + BLOCK_DIAMETER / 2 + BLOCK_DIAMETER * j,(float)BLOCK_DIAMETER * i,
							BLOCK_DIAMETER / 2,BLOCK_DIAMETER / 2}
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
			buttons[i].entity.position.y += sin(clock() / PI / 300 - i / 2.0) * 5;
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
			gameui_->Reset();
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
				gameui_->Update();

				//�S�Ẵu���b�N���X�V
				for (int i = 0; i < blocks.size(); i++)
				{
					blocks[i].rigidBody.gameObject.graphNum = blocksSprite[blocks[i].blockType];

					//����
					blocks[i].rigidBody.gameObject.entity.position.y =
						min(blocks[i].rigidBody.gameObject.entity.position.y + BLOCK_DIAMETER / 10, BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1));

					unsigned int landingBlockIndex = min(blocks[i].landingBlockIndex, blocks.size() - 1);
					Vector2D pos = blocks[i].rigidBody.gameObject.entity.position;
					Vector2D targetPos = blocks[landingBlockIndex].rigidBody.gameObject.entity.position;
					if (pos.x == targetPos.x
						&& pos.y < targetPos.y && pos.y > targetPos.y - BLOCK_DIAMETER)
					{
						//�x��������ꍇ�͌y�ʏ���
						blocks[i].rigidBody.gameObject.entity.position.y = targetPos.y - BLOCK_DIAMETER;
					}
					else
					{
						//�x���𖳂������琸������
						for (int j = 0; j < blocks.size(); j++)
						{
							pos = blocks[i].rigidBody.gameObject.entity.position;
							targetPos = blocks[j].rigidBody.gameObject.entity.position;
							if (pos.x == targetPos.x
								&& pos.y < targetPos.y && pos.y > targetPos.y - BLOCK_DIAMETER)
							{
								blocks[i].rigidBody.gameObject.entity.position.y = targetPos.y - BLOCK_DIAMETER;
								blocks[i].landingBlockIndex = j;
							}
						}
					}

					if (blocks[i].blockType == sandblock)
					{
						//���u���b�N�����@�ɐG�ꂽ�����鏀��
						if (HitRectAndRect(blocks[i].rigidBody.gameObject.entity, player.rigidBody.gameObject.entity))
						{
							blocks[i].status = "touched";
						}
						else
						{
							//����鏀�����o���Ă����Ԃō��u���b�N�����@�ɐG��Ă��Ȃ���Ԃ�4�t���[���i�o�b�t�@�̂��߁j������������
							if (blocks[i].status == "touched")
							{
								blocks[i].status = "prepareBreak3";
							}
							else if (blocks[i].status == "prepareBreak3")
							{
								blocks[i].status = "prepareBreak2";
							}
							else if (blocks[i].status == "prepareBreak2")
							{
								blocks[i].status = "prepareBreak";
							}
							else if (blocks[i].status == "prepareBreak")
							{
								blocks[i].breaked = true;
							}
						}
					}
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
				camPosition = Vector2D{ 0,player.rigidBody.gameObject.entity.position.y };

				vector<GameObject> edgeWall = {
					GameObject{Rect{Vector2D{-WIN_WIDTH / 2,0} + camPosition,{0,WIN_HEIGHT} }},
					GameObject{Rect{Vector2D{-WIN_WIDTH / 2 + GAME_LINE,0} + camPosition,{0,WIN_HEIGHT}}},
				};

				gameui_->digTimerT1 = (player.rigidBody.gameObject.entity.position.y + player.rigidBody.gameObject.entity.scale.y * 2.0f) / (player.rigidBody.gameObject.entity.scale.y * 2.0f);
				
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

			//�܂��͔w�i��`��(�X�N���[���t���@�摜�̏c����WIN_HEIGHT�̎O�{�ɒ����Ȃ�����)
			DrawGraph(0, -((int)camPosition.y % WIN_HEIGHT) - WIN_HEIGHT, backgroundSprite, true);
			//�S�Ẵu���b�N��`��
			for (int i = 0; i < blocks.size(); i++) {
				RenderObject(blocks[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//���@��`��
			RenderObject(player.rigidBody.gameObject, camPosition + camPosOffset);
			//���̃��C�������UI�]�[���Ȃ̂ł����������Ƀ{�b�N�XUI�����Ă�������
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);

			gameui_->Draw();
			DrawString(950, 30, "TIME", GetColor(0, 0, 0));
			DrawFormatString(1150, 155, GetColor(0, 0, 0), "%d", gameui_->autotimer);
			DrawString(950, 260, "DEPTH", GetColor(0, 0, 0));
			DrawString(1150,385,"m",GetColor(0, 0, 0));
			DrawString(950, 500, "�󂵂��u���b�N��", GetColor(0, 0, 0));

			//�X�N���[���`�F�b�N�p
			DrawFormatString(1200, 500, GetColor(122, 112, 122), "%d", -((int)camPosition.y % WIN_HEIGHT));
			DrawFormatString(1150, 180, GetColor(0, 0, 0), "%d", gameui_->digTimerT2);
			DrawFormatString(1200, 180, GetColor(0, 0, 0), "%d", gameui_->digTimerT1);
			break;
		case credit:
			//�N���W�b�g���
			DrawString(
				WIN_WIDTH / 4, 0,
				"\n\n�v���O���}�[\n�@�񃆃E\n�@�_����\n\n�`�[�t�v���O���}�[\n�@�Ă�؂�\n\n�A�[�g\n�@�Ă�؂�\n\n�G�t�F�N�g\n�@�Ă�؂�\n\n�T�E���h�G�t�F�N�g\n�@�Ă�؂�\n\n�R���|�[�T�[\n�@�Ă�؂�",
				GetColor(0, 0, 0));
			DrawString(
				WIN_WIDTH / 2, 0,
				"\n\n�e�N�j�J���T�|�[�g\n�@�񃆃E\n�@�_����\n\n�r�W���A���A�h�o�C�U�[\n�@�Ă�؂�\n\n�G�O�[�N�e�B�u�v���f���[�T�[\n�@�Ă�؂�\n\n�f�B���N�^�[\n�@�Ă�؂�\n\n�����͂�\n�@�Ă�؂��Q�[���Y\n�@�`�[��GJ4\n\n\nTERAPETA GAMES / TEAM GJ4\nAll Rights Reserved.",
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
