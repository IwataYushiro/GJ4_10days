#include "DxLib.h"

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "xx2x_xx_�i�}�G: �^�C�g��";

// �E�B���h�E����
const int WIN_WIDTH = 1280;

// �E�B���h�E�c��
const int WIN_HEIGHT = 720;

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
	SetBackgroundColor(0x00, 0x00, 0x00);

	// DXlib�̏�����
	if (DxLib_Init() == -1) { return -1; }

	// (�_�u���o�b�t�@)�`���O���t�B�b�N�̈�͗��ʂ��w��
	SetDrawScreen(DX_SCREEN_BACK);

	// �摜�Ȃǂ̃��\�[�X�f�[�^�̕ϐ��錾�Ɠǂݍ���
	//�^�C�g�����(�^�C�g����ʂ���V���[�e�B���O�Q�[��)��BGM
	int titleGraph;
	titleGraph = LoadGraph("Resources/Textures/title.png");
	//�Q�[���V�[����ʂ�BGM
	int gameUiGraph;
	gameUiGraph = LoadGraph("Resources/Textures/gemeui.png");
	//�N���A���(�V���[�e�B���O�Q�[������N���A���)��BGM
	int clearGraph;
	clearGraph = LoadGraph("Resources/Textures/clear.png");

	// �Q�[�����[�v�Ŏg���ϐ��̐錾
	//�V�[���Ǘ�
	int title = 0;
	int gamemode = 1;
	int clear = 2;
	int sceneNo = title;
	
	//�V�[���p�̃^�C�}�[
	int sceneTimer[3] = {0};

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
		//�V�[���ڍs�Ǘ�
		if (keys[KEY_INPUT_RETURN] == 1) {
			if (sceneNo == title) {
				sceneTimer[title]++;
				sceneTimer[clear] = 0;
			}
			else if (sceneNo == gamemode) {
				sceneTimer[gamemode]++;
				sceneTimer[title] = 0;
			}
			else if (sceneNo == clear) {
				sceneTimer[clear]++;
				sceneTimer[gamemode] = 0;
			}
		}
		if (sceneTimer[title] >= 30) {
			sceneNo = gamemode;
		}
		else if (sceneTimer[gamemode] >= 30) {
			sceneNo = clear;
		}
		else if (sceneTimer[clear] >= 30) {
			sceneNo = title;
		}


		// �`�揈��
		if (sceneNo == 0) {
			DrawGraph(0, 0, titleGraph, true);
		}
		else if (sceneNo == 1) {
			DrawGraph(0, 0, gameUiGraph, true);
		}
		else if (sceneNo == 2) {
			DrawGraph(0, 0, clearGraph, true);
		}

		DrawFormatString(0, 0, GetColor(122, 122, 122), "sceneTimer[title] : %d", sceneTimer[title]);
		DrawFormatString(0, 30, GetColor(122, 122, 122), "sceneTimer[gamemode] : %d", sceneTimer[gamemode]);
		DrawFormatString(0, 60, GetColor(122, 122, 122), "sceneTimer[clear] : %d", sceneTimer[clear]);


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
