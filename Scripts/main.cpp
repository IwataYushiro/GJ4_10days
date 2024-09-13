#include "DxLib.h"
#include "KxLib.h"
#include <time.h>
#include <string>
#include "gameUi.h"
#include <random>

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "4005_�S�蓮�n�w����@";

//�u���b�N�̑傫��
const int BLOCK_DIAMETER = 64;
//����̐�
const int PLAYPART_WIDTH = 14;
//1���̏c�̒���
const int PLAYPART_HEIGHT = 25;
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
	float latestLandingHeight = 0;
	string status = "";
};

struct LiveEntity
{
	RigidBody rigidBody;
	int* sprites;
	float spriteIndex = 0;
	int insideBlockCount;
	bool isLive = true;
	bool direction = true;
	int stuckFrameCount = 0;
	float despawnAnimProgress = 1;
	bool isDespawned = false;
};

struct Particle {
	RigidBody rigidBody;
	float drag = 1;
	float torque = 0;
	int maxLifeTime = 20;
	int lifetime = 20;
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

void LiveEntityUpdate(LiveEntity* liveEntity, std::vector<GameObject> blocks,
	int damageSound, int despawnSound, int* shardGraph, std::vector<Particle>& particles)
{
	//��������
	RigidBodyUpdate(liveEntity->rigidBody, { 0,1 }, { 0.5,1 }, blocks);

	//�������x�����i�u���b�N�Ɠ����ɂ���j
	liveEntity->rigidBody.movement.y = min(liveEntity->rigidBody.movement.y, BLOCK_DIAMETER / 10);

	liveEntity->rigidBody.gameObject.graphLocalPos =
	{ 0,-32 + liveEntity->rigidBody.gameObject.entity.scale.x };

	//�����Ă�����
	if (liveEntity->isLive)
	{
		liveEntity->despawnAnimProgress = 1;

		//�u���b�N�̒��ɉ������܂�Ă��邩����
		bool isInsideBlock = false;
		for (int i = 0; i < blocks.size(); i++)
		{
			if (HitRectAndPoint(blocks[i].entity,
				Vector2D{ liveEntity->rigidBody.gameObject.entity.position.x,
				liveEntity->rigidBody.gameObject.entity.position.y }))
			{
				isInsideBlock = true;
				break;
			}
		}
		//�����u���b�N�̒��ɉ������܂ꂽ��Ԃ�6�t���[���������玀��
		if (isInsideBlock)
		{
			liveEntity->insideBlockCount++;
			if (liveEntity->insideBlockCount >= 6)
			{
				liveEntity->isLive = false;
			}
		}
		else
		{
			liveEntity->insideBlockCount = 0;
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
			liveEntity->stuckFrameCount = min(max(0, liveEntity->stuckFrameCount), 6);

			if (liveEntity->stuckFrameCount >= 6)
			{
				liveEntity->direction = !liveEntity->direction;
				liveEntity->rigidBody.gameObject.dir = !liveEntity->rigidBody.gameObject.dir;
				liveEntity->stuckFrameCount = 0;
			}

			//�O�i
			float playerMoveForce = 1;
			if (!liveEntity->direction)
			{
				playerMoveForce *= -1;
			}
			liveEntity->rigidBody.movement.x += playerMoveForce;

			liveEntity->spriteIndex = fmodf(liveEntity->spriteIndex + 0.5f, 6);
		}
		else
		{
			//�󒆂ɂ��鎞
			liveEntity->stuckFrameCount = 0;
			liveEntity->spriteIndex = fmodf(liveEntity->spriteIndex + 1.4f, 6);
		}
	}
	else
	{
		//���񂾂Ƃ�

		liveEntity->spriteIndex = 6;

		liveEntity->rigidBody.gameObject.graphLocalPos.x =
			sinf(clock() / 10) * max(liveEntity->despawnAnimProgress - 0.75f, 0) * 20;

		float prevDespawnAnimProgress = liveEntity->despawnAnimProgress;
		liveEntity->despawnAnimProgress -= 0.015f;

		if (prevDespawnAnimProgress == 1)
		{
			//�_���[�W����炷
			PlaySoundMem(damageSound, DX_PLAYTYPE_BACK, true);
		}

		if (prevDespawnAnimProgress > 0.5f && liveEntity->despawnAnimProgress <= 0.5f)
		{
			//�j�Ђ𐶐�
			float rotRand = rand();
			for (int j = 0; j < 9; j++)
			{
				float angleRad = PI * 2 / 9 * j + rotRand;
				int spriteIndex = rand() % 2;
				if (j % 2 == 0)
				{
					spriteIndex += 2;
				}
				particles.push_back(Particle{ RigidBody{GameObject{
					liveEntity->rigidBody.gameObject.entity,shardGraph[spriteIndex]},
					Vector2D{sinf(angleRad),cosf(angleRad)} *15},
					0.5f + (rand() % 20) * 0.01f,(float)(rand() % 7 - 3) * 0.1f });
			}

			PlaySoundMem(despawnSound, DX_PLAYTYPE_BACK, true);
		}
	}
	liveEntity->rigidBody.gameObject.graphNum = liveEntity->sprites[(int)liveEntity->spriteIndex];

	liveEntity->despawnAnimProgress = min(max(0, liveEntity->despawnAnimProgress), 1);
	liveEntity->isDespawned = !liveEntity->isLive && liveEntity->despawnAnimProgress <= 0;
}

void GenerateLevel(std::vector<Block>& blocks, int stageLevel)
{
	//�u���b�N�����p�̃����_��
	//�����_������(int)
	std::random_device seedBlock;
	std::mt19937_64 engineBlock(seedBlock());
	std::uniform_real_distribution<> distTappableBlock(0, 3);
	std::uniform_real_distribution<> distUnTappableBlock(3, 5);
	std::uniform_real_distribution<> distRandomGenerate(0, 5);
	std::uniform_real_distribution<> distRandomCount(0, 5);

	//�u���b�N���������A����
	int tappableBlockCount = 0;
	int unTappableBlockCount = 0;
	int tappableBlockCount2 = 0;
	int randomGenerateCount = 0;
	blocks = {};
	for (int i = -1; i < PLAYPART_HEIGHT; i++)
	{
		for (int j = 0; j < PLAYPART_WIDTH; j++)
		{
			if (tappableBlockCount <= 0 && unTappableBlockCount <= 0 && tappableBlockCount2 <= 0  && randomGenerateCount <= 0)
			{
				tappableBlockCount = static_cast<int>(distRandomCount(engineBlock)) + PLAYPART_WIDTH / 3;
				unTappableBlockCount = static_cast<int>(distRandomCount(engineBlock)) + min(max(0, (stageLevel - 1) * 4), PLAYPART_WIDTH * 3);
				tappableBlockCount2 = static_cast<int>(distRandomCount(engineBlock)) + PLAYPART_WIDTH / 3;
				randomGenerateCount = static_cast<int>(distRandomCount(engineBlock)) + min(max(0, (stageLevel - 1) * 8), PLAYPART_WIDTH * 3);
			}

			BlockType currentBlockType = weakblock;
			if (i == PLAYPART_HEIGHT - 1)
			{
				currentBlockType = static_cast<BlockType>(distTappableBlock(engineBlock));
			}
			else if (i == -1)
			{
				currentBlockType = frameblock;
			}
			else if(i > 0)
			{
				if (tappableBlockCount > 0)
				{
					currentBlockType = static_cast<BlockType>(distTappableBlock(engineBlock));
					tappableBlockCount--;
				}
				else if (unTappableBlockCount > 0)
				{
					currentBlockType = static_cast<BlockType>(distUnTappableBlock(engineBlock));
					unTappableBlockCount--;
				}
				else if (tappableBlockCount2 > 0)
				{
					currentBlockType = static_cast<BlockType>(distTappableBlock(engineBlock));
					tappableBlockCount2--;
				}
				else
				{
					currentBlockType = static_cast<BlockType>(distRandomGenerate(engineBlock));
					randomGenerateCount--;
				}
			}

			blocks.push_back(Block{ RigidBody{GameObject{
				Rect{(float)-WIN_WIDTH / 2 + BLOCK_DIAMETER / 2 + BLOCK_DIAMETER * j,(float)BLOCK_DIAMETER * i,
				BLOCK_DIAMETER / 2,BLOCK_DIAMETER / 2}
			}},currentBlockType });
			blocks.back().latestLandingHeight = blocks.back().rigidBody.gameObject.entity.position.y;
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
	SetBackgroundColor(0xff, 0xd8, 0x86);

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
	const int logoGraph = LoadGraph("Resources/Textures/ALHATERAPETAGAMES_logo.png");
	//�^�C�g����ʔw�i
	const int bgGraph = LoadGraph("Resources/Textures/backGround.png");
	//�I�v�V������ʔw�i
	const int optionBGGraph = LoadGraph("Resources/Textures/optionBackGround.png");
	//�^�C�g�����
	const int titleGraph = LoadGraph("Resources/Textures/title.png");
	//�`���[�g���A��
	const int tutorialGraph0 = LoadGraph("Resources/Textures/howToPlay0.png");
	const int tutorialGraph1 = LoadGraph("Resources/Textures/howToPlay1.png");
	//���@
	int playerSprites[7];
	LoadDivGraph("Resources/Textures/miniminiKX.png", 7,
		4, 4,
		64, 64, playerSprites);
	//�u���b�N�e��
	const int blocksSprite[] = {
		LoadGraph("Resources/Textures/weakBlock.png"),
		LoadGraph("Resources/Textures/sandBlock.png"),
		LoadGraph("Resources/Textures/frameBlock.png"),
		LoadGraph("Resources/Textures/unTappableBlock.png"),
		LoadGraph("Resources/Textures/lethalBlock.png"),
	};
	//�{���u���b�N�̔j��
	int weakBlockShards[8];
	LoadDivGraph("Resources/Textures/weakBlockShard.png", 8,
		4, 2,
		32, 32, weakBlockShards);
	//���u���b�N�̔j��
	int sandBlockShards[8];
	LoadDivGraph("Resources/Textures/sandBlockShard.png", 8,
		4, 2,
		32, 32, sandBlockShards);
	//�t���[���u���b�N�̔j��
	int frameBlockShards[8];
	LoadDivGraph("Resources/Textures/frameBlockShard.png", 8,
		4, 2,
		32, 32, frameBlockShards);
	//�h�ǃu���b�N�̔j��
	int unTappableBlockShards[8];
	LoadDivGraph("Resources/Textures/unTappableBlockShard.png", 8,
		4, 2,
		32, 32, unTappableBlockShards);
	//���[�T���u���b�N�̔j��
	int lethalBlockShards[8];
	LoadDivGraph("Resources/Textures/lethalBlockShard.png", 8,
		4, 2,
		32, 32, lethalBlockShards);
	//���@�̔j��
	int despawnShards[4];
	LoadDivGraph("Resources/Textures/miniminiKXShard.png", 4,
		2, 2,
		32, 32, despawnShards);
	//�v���C�p�[�g�w�i
	const int playPartBGGraph = LoadGraph("Resources/Textures/playPartBackGround.png");

	//�{�^����������
	const int buttonPushSound = LoadSoundMem("Resources/SE/buttonPush.wav");
	//�u���b�N���󂷉�
	const int blockBreakSound = LoadSoundMem("Resources/SE/breakBlock.wav");
	//�󂹂Ȃ��u���b�N��������
	const int reflectSound = LoadSoundMem("Resources/SE/reflect.wav");
	//�_���[�W��
	const int damageSound = LoadSoundMem("Resources/SE/damage.wav");
	//���񂾂Ƃ��̉�
	const int despawnSound = LoadSoundMem("Resources/SE/despawn.wav");

	//���̃Q�[���ŗ����S�Ă�BGM�i�Ȃ𗬂������Ȃ��Ƃ��̓C���f�b�N�X0�Ԃ��w��j
	const int audioClip[] = {
		0,
		LoadSoundMem("Resources/BGM/title.mp3"),
		LoadSoundMem("Resources/BGM/howToPlay.mp3"),
		LoadSoundMem("Resources/BGM/option.mp3"),
		LoadSoundMem("Resources/BGM/game.mp3"), };

	// �Q�[�����[�v�Ŏg���ϐ��̐錾

	//�}�E�X���͂������t�B�[���h
	MouseInputData mouseInputData;

	//���݂̃V�[��
	Scene currentScene = logo;
	//�J�ڂ��悤�Ƃ��Ă��鎟�̃V�[��
	Scene nextScene = logo;
	//�V�[���J�ڗp�̃^�C�}�[
	float sceneTransitionProgress = 0;
	//�V�[���̏��������K�v�Ȃ炱���true��
	bool sceneInit = false;
	//�t�F�[�h�C���A�E�g(�C���A�A�E�g)
	int fadeInOutCount = 0;
	//�������Ȃ̃C���f�b�N�X
	int bgmNum = 0;
	//BGM�ASE�̃{�����[��
	float bgmVolume = 1;
	float seVolume = 1;

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
	//�p�[�e�B�N��
	vector<Particle> particles = {};

	//�X�e�[�W�̓�Փx�A���̈�ԉ��܂œ��B���邽�тɂP���オ��
	unsigned int stageLevel = 0;

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


		if (nextScene == currentScene)
		{
			if (sceneTransitionProgress >= 10 && sceneTransitionProgress < 20)
			{
				sceneTransitionProgress++;
			}
			else
			{
				sceneTransitionProgress = 0;
			}
		}
		else
		{
			//�V�[���؂�ւ��̃J�E���g�_�E��
			sceneTransitionProgress++;
			if (sceneTransitionProgress >= 10) {
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

				gameui_->Reset();
				//�|�[�Y����
				isPause = false;

				//���x�������Z�b�g
				stageLevel = 0;

				//���@���������W��
				player = LiveEntity{ RigidBody{ GameObject{ Rect{-WIN_WIDTH / 2 + GAME_LINE / 2,-WIN_HEIGHT / 2 - BLOCK_DIAMETER * 2,20,20},0,1,{0,-12}} },playerSprites };

				//�n�w����
				GenerateLevel(blocks, stageLevel);
				break;
			}
			sceneInit = false;
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
					Button{Rect{GAME_LINE / 2, 160,150,50},"�Â���","RESUME"},
					Button{Rect{GAME_LINE / 2, 460,150,50},"��߂�","QUIT"},
				};
			}
			else if (player.isDespawned && gameui_->isrankEnd)
			{
				buttons = {
					Button{Rect{GAME_LINE / 4, 560,150,50},"���g���C","TRY AGAIN"},
					Button{Rect{GAME_LINE - (GAME_LINE / 4), 560,150,50},"�^�C�g����","QUIT"},
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

			//�Ȃ��w�肵�Ȃ�
			bgmNum = 0;

			//���N���b�N�Ń^�C�g����
			if (!mouseInputData.click && mouseInputData.preClick)
			{
				nextScene = title;
			}
			break;
		case title:
			//�^�C�g�����

			//�^�C�g���̋Ȃ��w��
			bgmNum = 1;

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

			//�v���C�p�[�g�̋Ȃ��w��
			bgmNum = 4;
			//�Ȃ̉��ʂ�傫��
			ChangeVolumeSoundMem(255, audioClip[2]);

			if (!isPause)
			{
				if (player.isLive)
				{
					if (player.rigidBody.gameObject.entity.position.y >= BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1))
					{
						//���x���A�b�v
						stageLevel++;

						//���@�����
						player.rigidBody.gameObject.entity.position.y = -WIN_HEIGHT / 2 - BLOCK_DIAMETER * 2;

						//�n�w����
						GenerateLevel(blocks, stageLevel);
					}

					gameui_->Update();

					//�S�Ẵu���b�N���X�V
					for (int i = 0; i < blocks.size(); i++)
					{
						blocks[i].rigidBody.gameObject.graphNum = blocksSprite[blocks[i].blockType];
						blocks[i].rigidBody.gameObject.graphLocalPos = blocks[i].rigidBody.gameObject.graphLocalPos * 0.3f;

						//����
						blocks[i].rigidBody.gameObject.entity.position.y =
							min(blocks[i].rigidBody.gameObject.entity.position.y + BLOCK_DIAMETER / 10, BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1));

						unsigned int landingBlockIndex = min(blocks[i].landingBlockIndex, blocks.size() - 1);
						Vector2D pos = blocks[i].rigidBody.gameObject.entity.position;
						Vector2D targetPos = blocks[landingBlockIndex].rigidBody.gameObject.entity.position;
						if (pos.y >= BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1)
							|| (pos.x == targetPos.x
								&& pos.y < targetPos.y && pos.y > targetPos.y - BLOCK_DIAMETER))
						{
							//�x��������ꍇ�͌y�ʏ���
							if (pos.y < BLOCK_DIAMETER * (PLAYPART_HEIGHT - 1))
							{
								blocks[i].rigidBody.gameObject.entity.position.y = targetPos.y - BLOCK_DIAMETER;
							}
							blocks[i].latestLandingHeight = blocks[i].rigidBody.gameObject.entity.position.y;
							//���鏀�����o���Ă����Ԃŉ󂹂Ȃ��u���b�N�����n���Ă��������
							if (blocks[i].blockType == untappableblock || blocks[i].blockType == lethalblock)
							{
								if (blocks[i].status == "prepareBreak")
								{
									blocks[i].breaked = true;
								}
							}
						}
						else
						{
							//�x���𖳂������琸������

							//���̃u���b�N��T��
							for (int j = 0; j < blocks.size(); j++)
							{
								pos = blocks[i].rigidBody.gameObject.entity.position;
								targetPos = blocks[j].rigidBody.gameObject.entity.position;
								if (pos.x == targetPos.x
									&& pos.y < targetPos.y && pos.y > targetPos.y - BLOCK_DIAMETER)
								{
									blocks[i].rigidBody.gameObject.entity.position.y = targetPos.y - BLOCK_DIAMETER;
									blocks[i].landingBlockIndex = j;

									break;
								}
							}

							//�󂹂Ȃ��u���b�N���m���ڐG��������鏀��
							if (blocks[i].blockType == untappableblock || blocks[i].blockType == lethalblock)
							{
								for (int j = 0; j < blocks.size(); j++)
								{
									pos = blocks[i].rigidBody.gameObject.entity.position;
									targetPos = blocks[j].rigidBody.gameObject.entity.position;
									if (i != j && blocks[i].rigidBody.gameObject.entity.position.y - blocks[i].latestLandingHeight > BLOCK_DIAMETER / 1.1f
										&& (blocks[j].blockType == untappableblock || blocks[j].blockType == lethalblock)
										&& VectorScale(pos - targetPos) <= BLOCK_DIAMETER * 1.2f)
									{
										blocks[i].status = "prepareBreak";
										blocks[j].status = "prepareBreak";
									}
								}
							}
						}

						if (blocks[i].blockType == sandblock)
						{
							//���u���b�N�����@�ɐG�ꂽ�����鏀��
							if (HitRectAndRect(blocks[i].rigidBody.gameObject.entity, player.rigidBody.gameObject.entity)
								&& VectorScale(blocks[i].rigidBody.gameObject.entity.position - player.rigidBody.gameObject.entity.position) <= BLOCK_DIAMETER)
							{
								blocks[i].status = "touched";
								blocks[i].rigidBody.gameObject.graphLocalPos =
									NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 1;
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
									blocks[i].rigidBody.gameObject.graphLocalPos =
										NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 4;
								}
								else if (blocks[i].status == "prepareBreak2")
								{
									blocks[i].status = "prepareBreak";
									blocks[i].rigidBody.gameObject.graphLocalPos =
										NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 4;
								}
								else if (blocks[i].status == "prepareBreak")
								{
									blocks[i].breaked = true;
								}
							}
						}

						//�N���b�N���ꂽ����������A�󂹂Ȃ��u���b�N�͏������o���N��������
						if (HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.position + (camPosition + camPosOffset))
							&& HitRectAndPoint(blocks[i].rigidBody.gameObject.entity, mouseInputData.pin + (camPosition + camPosOffset))
							&& !mouseInputData.click && mouseInputData.preClick)
						{
							if (blocks[i].blockType == untappableblock || blocks[i].blockType == lethalblock)
							{
								PlaySoundMem(reflectSound, DX_PLAYTYPE_BACK, true);
								blocks[i].rigidBody.gameObject.graphLocalPos =
									NormalizedVector(Vector2D{ (float)(rand() % 4) - 1.5f,(float)(rand() % 4) - 1.5f }) * 10;
							}
							else
							{
								blocks[i].breaked = true;
							}
						}

						//���[�T���u���b�N�Ɏ��@���G�ꂽ�玀��
						if (blocks[i].blockType == lethalblock
							&& HitRectAndRect(blocks[i].rigidBody.gameObject.entity, player.rigidBody.gameObject.entity)
							&& VectorScale(blocks[i].rigidBody.gameObject.entity.position - player.rigidBody.gameObject.entity.position) <= BLOCK_DIAMETER)
						{
							player.isLive = false;
						}
					}
					//�����������o�����u���b�N��S������
					for (int i = 0; i < blocks.size(); i++)
					{
						if (blocks[i].breaked)
						{
							gameui_->blockCountT1++;

							//�j�Ђɓ\��t����摜�����߂�
							int* shardGraph;
							switch (blocks[i].blockType)
							{
							case weakblock:
								shardGraph = weakBlockShards;
								break;
							case sandblock:
								shardGraph = sandBlockShards;
								break;
							case frameblock:
								shardGraph = frameBlockShards;
								break;
							case untappableblock:
								shardGraph = unTappableBlockShards;
								break;
							case lethalblock:
								shardGraph = lethalBlockShards;
								break;
							default:
								shardGraph = frameBlockShards;
								break;
							}
							//�j�Ђ𐶐�
							float rotRand = rand();
							for (int j = 0; j < 9; j++)
							{
								float angleRad = PI * 2 / 9 * j + rotRand;
								int spriteIndex = rand() % 4;
								if (j % 2 == 0)
								{
									spriteIndex += 4;
								}
								particles.push_back(Particle{ RigidBody{GameObject{
									blocks[i].rigidBody.gameObject.entity,shardGraph[spriteIndex]},
									Vector2D{sinf(angleRad),cosf(angleRad)} *15},
									0.5f + (rand() % 20) * 0.01f,(float)(rand() % 7 - 3) * 0.1f });
							}

							PlaySoundMem(blockBreakSound, DX_PLAYTYPE_BACK, true);
							blocks.erase(blocks.begin() + i);
							i--;
						}
					}

					gameui_->depthT1 =
						stageLevel * PLAYPART_HEIGHT + max((player.rigidBody.gameObject.entity.position.y) / BLOCK_DIAMETER + 1, 0);

					//�{�^�������������̏���
					if (IsButtonClicked(buttons, 0))
					{
						//�|�[�Y����
						isPause = true;
					}
				}
				else if (player.isDespawned)
				{
					//���@���S��
					//������
					if (IsButtonClicked(buttons, 0))
					{
						//���Z�b�g
						sceneInit = true;
					}
					//�I���
					if (IsButtonClicked(buttons, 1))
					{
						//�^�C�g����
						nextScene = title;
					}

					//�Ȃ̉��ʂ�������
					ChangeVolumeSoundMem(128, audioClip[2]);
				}

				//�S�Ẵp�[�e�B�N�����X�V
				for (int i = 0; i < particles.size(); i++)
				{
					RigidBodyUpdate(particles[i].rigidBody, { 0,0 }, { particles[i].drag,particles[i].drag });
					particles[i].rigidBody.gameObject.rot += particles[i].torque;
					particles[i].rigidBody.gameObject.graphScale = powf((float)particles[i].lifetime / particles[i].maxLifeTime, 0.4f);
					particles[i].lifetime--;
				}
				//�������s�����p�[�e�B�N��������
				for (int i = 0; i < particles.size(); i++)
				{
					if (particles[i].lifetime <= 0)
					{
						particles.erase(particles.begin() + i);
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
				LiveEntityUpdate(&player, liveEntityWalls, damageSound, despawnSound, despawnShards, particles);

				//�J�����Ǐ]
				camPosition = Vector2D{ 0,player.rigidBody.gameObject.entity.position.y };

				//���[�̕ǂ�Ǐ]������
				edgeWall = {
					GameObject{Rect{Vector2D{-WIN_WIDTH / 2,0} + camPosition,{0,WIN_HEIGHT}}},
					GameObject{Rect{Vector2D{-WIN_WIDTH / 2 + GAME_LINE,0} + camPosition,{0,WIN_HEIGHT}}},
				};
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

				//�Ȃ̉��ʂ�������
				ChangeVolumeSoundMem(128, audioClip[2]);
			}

			break;
		case credit:
			//�N���W�b�g���

			//�N���W�b�g�̋Ȃ��w��
			bgmNum = 3;

			//�{�^�������������̏���
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		case howtoplay:
			//�V�ѕ��������

			//�V�ѕ������̋Ȃ��w��
			bgmNum = 2;

			//�{�^�������������̏���
			if (IsButtonClicked(buttons, 0))
			{
				nextScene = title;
			}
			break;
		}

		//�Ȃ��Đ��A�����ɗ����ׂ��łȂ��Ȃ��~�߂�
		if (!CheckSoundMem(audioClip[bgmNum])) {
			PlaySoundMem(audioClip[bgmNum], DX_PLAYTYPE_BACK, true);
		}
		for (int i = 0; i < sizeof(audioClip) / sizeof(*audioClip); i++) {
			if (i != bgmNum) {
				StopSoundMem(audioClip[i]);
			}
		}

		// �`�揈��

		// ��ʂ̔w�i�F��ݒ肷��
		SetBackgroundColor(0xff, 0xd8, 0x86);

		//�w�i�摜
		int currentBGGraph = bgGraph;

		if (currentScene == credit)
		{
			SetBackgroundColor(0x86, 0xc7, 0xff);
			currentBGGraph = optionBGGraph;
		}

		DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2 - fmodf(clock() * 0.00005f,1) * 320 + 160, 1, 0, currentBGGraph, TRUE);

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
				WIN_WIDTH / 7 * 3, WIN_HEIGHT / 3 * 2,
				"TEAM 4005",
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
			//�����\��
			DrawString(
				WIN_WIDTH / 3, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "2024 ALHA TERAPETA GAMES / TEAM 4005",
				GetColor(0, 0, 0));
			//�o�[�W�����\��
			DrawString(
				WIN_WIDTH / 6 * 5, WIN_HEIGHT - (FONT_SIZE * 2 + 10), "Ver. 1.0.0",
				GetColor(0, 0, 0));
			break;
		case playpart:
			//�v���C�p�[�g

			//�܂��͔w�i��`��(�X�N���[���t��)
			DrawRotaGraph(GAME_LINE / 2, WIN_HEIGHT / 2 - fmodf(camPosition.y + 640, 320) + 160, 1, 0, playPartBGGraph, true);
			//�S�Ẵu���b�N��`��
			for (int i = 0; i < blocks.size(); i++) {
				RenderObject(blocks[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//�S�Ẵp�[�e�B�N����`��
			for (int i = 0; i < particles.size(); i++) {
				RenderObject(particles[i].rigidBody.gameObject, camPosition + camPosOffset);
			}
			//���@��`��
			if (player.despawnAnimProgress > 0.5f)
			{
				RenderObject(player.rigidBody.gameObject, camPosition + camPosOffset);
			}


			//���̃��C�������UI�]�[���Ȃ̂ł����������Ƀ{�b�N�XUI�����Ă�������
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, 0xffffff, TRUE);
			DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xd8, 0x86), FALSE);
			DrawTriangle(
				WIN_WIDTH,0,
				WIN_WIDTH,WIN_HEIGHT,
				GAME_LINE,WIN_HEIGHT,
				GetColor(0xff, 0xd8, 0x86), TRUE);
			gameui_->Draw();
			if (player.isDespawned)
			{
				gameui_->DrawRank();
			}
			break;
		case credit:
			//�N���W�b�g���
			DrawString(
				WIN_WIDTH / 4, 0,
				"\n\n�v���O���}�[\n�@�񃆃E\n�@�_����\n\n�`�[�t�v���O���}�[\n�@�Ă�؂�\n\n�G�t�F�N�g\n�@�Ă�؂�\n\n�L�����N�^�[�f�U�C��\n�@�����\n\n�A�[�g\n�@�����\n\n�T�E���h�G�t�F�N�g\n�@�����\n\n�R���|�[�T�[\n�@�����\n\nUI�f�U�C��\n�@�_����\n�@�Ă�؂�",
				GetColor(0, 0, 0));
			DrawString(
				WIN_WIDTH / 2, 0,
				"\n\n�e�N�j�J���T�|�[�g\n�@�񃆃E\n�@�_����\n\n�r�W���A���A�h�o�C�U�[\n�@�Ă�؂�\n\n�G�O�[�N�e�B�u�v���f���[�T�[\n�@�Ă�؂�\n\n�f�B���N�^�[\n�@�Ă�؂�\n\n�����͂�\n�@����͂Ă�؂��Q�[���Y\n�@�`�[��4005\n\n\nALHA TERAPETA GAMES / TEAM 4005\nAll Rights Reserved.",
				GetColor(0, 0, 0));
			break;
		case howtoplay:
			DrawRotaGraph(WIN_WIDTH / 4, WIN_HEIGHT / 2, 1, 0, tutorialGraph0, true);
			DrawRotaGraph(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 2, 1, 0, tutorialGraph1, true);
			break;
		}

		//�S�Ẵ{�^����`��
		for (int i = 0; i < buttons.size(); i++)
		{
			DrawButton(buttons[i]);
		}

		//�V�[���J�ڃV���b�^�[
		DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2,
			WIN_WIDTH / 64 * sinf(sceneTransitionProgress / 20.0f * PI),
			(sceneTransitionProgress - 10)/10.0f, blocksSprite[0], true);

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
