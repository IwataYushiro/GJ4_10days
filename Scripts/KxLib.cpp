// -------------------------------------------------------------------------------
// 
// 		KX���C�u����		cpp�t�@�C��
// 
// 				Ver 20211115.0
// 
// -------------------------------------------------------------------------------

#include "KxLib.h"
#include "DxLib.h"
#include <math.h>

using namespace std;

///<summary>
///�Q�_�Ԃ̋���
///<para>...</para>
///<para>a,b = �Q�_�̍��W</para>
/// </summary>
double VectorScale(Vector2D a, Vector2D b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

///<summary>
///�Q�_�Ԃ̃x�N�g���𐳋K��
///<para>...</para>
///<para>a,b = �Q�_�̍��W</para>
/// </summary>
Vector2D NormalizedVector(Vector2D a, Vector2D b) {
	if (VectorScale(a, b) != 0) {
		return Vector2D{ (b.x - a.x) / VectorScale(a, b),(b.y - a.y) / VectorScale(a, b) };
	}
	return Vector2D{ 0,0 };
}

///<summary>
//�x���@�̊p�x�����W�A���p�ɕϊ�����
///<para>...</para>
///<para>degree = �x���@�̊p�x</para>
/// </summary>
float DegreeToRad(float degree) {
	return degree * PI / 180;
}

///<summary>
//���W�A���p��x���@�̊p�x�ɕϊ�����
///<para>...</para>
///<para>rad = ���W�A���p</para>
/// </summary>
float RadToDegree(float rad) {
	return rad / PI * 180;
}

///<summary>
///<para>���͂������W�A���p�̕����������������P�̃x�N�g�����o�͂���</para>
///<para>...</para>
///<para>rad = ���W�A���p</para>
/// </summary>
Vector2D RadToPos(double rad) {
	Vector2D ret;
	ret.x = sin(rad);
	ret.y = -cos(rad);
	return ret;
}

///<summary>
///<para>�������̍��W����������̍��W�����������̊p�x</para>
///<para>...</para>
///<para>nowPos = ��ƂȂ���W</para>
///<para>targetPos = �����Ώۂ̍��W</para>
/// </summary>
double PosToRad(Vector2D nowPos, Vector2D targetPos) {
	return atan2(targetPos.x - nowPos.x, nowPos.y - targetPos.y);
}

///<summary>
///<para>�V�[����؂�ւ��鏀���i�V�[����؂�ւ��鏈��������ꏊ�ɒu���j</para>
///<para>������g���O�Ɍ��݂̃V�[�����Ǘ�����ϐ���</para>
///<para>���̃V�[�����i�[����ϐ���</para>
///<para>�V�[���ύX�܂ł̎c�莞�Ԃ��Ǘ�����ϐ���p�ӂ���</para>
///<para>...</para>
///<para>progress = �V�[���ύX�܂ł̎c�莞�Ԃ��Ǘ�����ϐ�</para>
///<para>nextScene = ���̃V�[�����i�[����ϐ�</para>
///<para>setNextScene = ���̃V�[����\�����l�𒼐ړ���</para>
///</summary>
void SetNextScene(float& progress, int& nextscene, int setNextScene) {
	nextscene = setNextScene;
	if (progress >= 1) {
		progress = 0;
	}
}

///<summary>
///<para>�V�[����؂�ւ��邽�߂̍X�V�����i�X�V���������̉����ɒu���j</para>
///<para>SetNextScene()�ƕ��p����</para>
///<para>...</para>
///<para>progress = �V�[���ύX�܂ł̎c�莞�Ԃ��Ǘ�����ϐ�</para>
///<para>scene = ���݂̃V�[�����Ǘ�����ϐ�</para>
///<para>nextScene = ���̃V�[�����i�[����ϐ�</para>
///</summary>
void SceneUpdate(float& progress, int& scene, int nextScene) {
	if (progress < 1) {
		progress += 0.01;
	}
}

///<summary>
///GameObject��`�悷��
///<para>...</para>
///<para>obj = �摜�Ƌ�`�̏��̉�</para>
///<para>scroll = �J�����̈ʒu</para>
/// </summary>
void RenderObject(GameObject obj, Vector2D scroll) {
	DrawRotaGraph(obj.entity.x - scroll.x, obj.entity.y + scroll.y, obj.graphScale, obj.rot, obj.graphNum, true, obj.dir);
}

///<summary>
///�z����̑SGameObject��`�悷��
///<para>...</para>
///<para>obj = �`�悷��GameObject�̔z��</para>
///<para>scroll = �J�����̈ʒu</para>
/// </summary>
void RenderAllObject(vector<GameObject> obj, Vector2D scroll) {
	for (int i = 0; i < obj.size(); i++) {
		RenderObject(obj[i], scroll);
	}
}

///<summary>
///RigidBody�ɏd�͂Ƌ�C��R��������
///<para>...</para>
///<para>rgd = �Ώۂ�RigidBody</para>
///<para>gravity = ������d�͂�x,y����</para>
///<para>drag = x,y�����̃X�s�[�h�ێ����i�O�`�P�̏�������͂��Ăˁj</para>
/// </summary>
void GravityAndDrag(RigidBody& rgd, Vector2D gravity, Vector2D drag) {
	rgd.movement.x += gravity.x;
	rgd.movement.y += gravity.y;
	rgd.movement.x *= drag.x;
	rgd.movement.y *= drag.y;
}

///<summary>
///�}�b�v�`�b�v����ɒn�`�u���b�N����ׂ�
///<para>...</para>
///<para>tileMap = �}�b�v�`�b�v���L���ꂽ�z��</para>
///<para>tileScale = ���������^�C���̑傫��</para>
///<para>tile = �n�`�u���b�N�ɓ\��t����摜�̔ԍ�</para>
///<para>blocks = �o���オ�����n�`�u���b�N���i�[����z��</para>
/// </summary>
void ConstructMap(vector<vector<int>> tileMap, const int tileScale, const int tile, vector<GameObject>& blocks, float graphScale) {
	blocks = {};
	for (int i = 0; i < tileMap.size(); i++) {
		for (int j = 0; j < tileMap[i].size(); j++) {
			if (tileMap[i][j] != 0) {
				blocks.push_back(GameObject{ rect{ j * (float)tileScale + (float)tileScale / 2, i * (float)tileScale + (float)tileScale / 2, (float)tileScale, (float)tileScale }, tile, graphScale });
			}
		}
	}
}

///<summary>
///��`���m���d�Ȃ��Ă��邩�ۂ�
///<para>...</para>
///<para>a,b = ������s���Q�̋�`</para>
/// </summary>
bool HitRectAndRect(rect a, rect b) {
	if (a.x - a.w * 0.5 <= b.x + b.w * 0.5 && a.x + a.w * 0.5 >= b.x - b.w * 0.5 && a.y - a.h * 0.5 <= b.y + b.h * 0.5 && a.y + a.h * 0.5 >= b.y - b.h * 0.5) {
		return true;
	}
	return false;
}

///<summary>
///�ǂ̕������Ώۂ̋�`�ɓ������Ă��邩
///<para>...</para>
///<para>obj = ��������I�u�W�F�N�g</para>
///<para>wall =�G��Ă���n�`�u���b�N</para>
/// </summary>
Vector2D CollisionInfoRectAndRect(GameObject obj, rect wall) {
	Vector2D ret = Vector2D{ 0,0 };
	obj.entity = rect{ (float)obj.beforePos.x,(float)obj.beforePos.y,obj.entity.w,obj.entity.h };
	if (abs(wall.y - obj.entity.y) * (wall.w + obj.beforeSca.x) < abs(wall.x - obj.entity.x) * (wall.h + obj.beforeSca.y)) {
		if (obj.entity.x <= wall.x) {
			ret.x = 1;
		}
		else {
			ret.x = -1;
		}
	}
	else {
		if (obj.entity.y <= wall.y) {
			ret.y = 1;
		}
		else {
			ret.y = -1;
		}
	}
	return ret;
}

///<summary>
///�Ώۂ̋�`�ɑ΂��ĉ����߂�
///<para>...</para>
///<para>obj = �����߂����I�u�W�F�N�g</para>
///<para>wall = obj�������߂��n�`�u���b�N</para>
/// </summary>
void CollisionRectAndRect(GameObject& obj, rect wall) {
	if (CollisionInfoRectAndRect(obj, wall).x != 0) {
		if (obj.entity.x <= wall.x) {
			obj.entity.x -= (obj.entity.x + obj.entity.w / 2.0) - (wall.x - wall.w / 2.0);
		}
		else {
			obj.entity.x -= (obj.entity.x - obj.entity.w / 2.0) - (wall.x + wall.w / 2.0);
		}
	}
	else {
		if (obj.entity.y <= wall.y) {
			obj.entity.y -= (obj.entity.y + obj.entity.h / 2.0) - (wall.y - wall.h / 2.0);
		}
		else {
			obj.entity.y -= (obj.entity.y - obj.entity.h / 2.0) - (wall.y + wall.h / 2.0);
		}
	}
}

///<summary>
///RigidBody��z����̑S�Ă̒n�`�u���b�N�ɑ΂��ĉ����߂�
///<para>...</para>
///<para>rgd = �����߂����RigidBody</para>
///<para>blocks = rgd�������߂��n�`�u���b�N�̔z��</para>
/// </summary>
void collideWall(RigidBody& rgd, vector<GameObject> blocks) {
	vector<GameObject> hittedBlocks = {};
	vector<GameObject> hittedEdgeBlocks = {};
	for (int i = 0; i < blocks.size(); i++) {
		if (HitRectAndRect(rgd.gameObject.entity, blocks[i].entity)) {
			hittedBlocks.push_back(blocks[i]);
		}
	}
	for (int i = 0; i < hittedBlocks.size(); i++) {
		float dist = -1;
		int target = 0;
		for (int j = 0; j < hittedBlocks.size(); j++) {
			if (VectorScale(Vector2D{ rgd.gameObject.entity.x ,rgd.gameObject.entity.y }, Vector2D{ hittedBlocks[j].entity.x ,hittedBlocks[j].entity.y }) < dist || dist < 0) {
				dist = VectorScale(Vector2D{ rgd.gameObject.entity.x ,rgd.gameObject.entity.y }, Vector2D{ hittedBlocks[j].entity.x ,hittedBlocks[j].entity.y });
				target = j;
			}
		}
		CollisionRectAndRect(rgd.gameObject, hittedBlocks[target].entity);
		if ((CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).x == 1 && rgd.movement.x > 0)
			|| (CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).x == -1 && rgd.movement.x < 0)) {
			rgd.movement.x *= 0.3;
		}
		if ((CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).y == 1 && rgd.movement.y > 0)
			|| (CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).y == -1 && rgd.movement.y < 0)) {
			rgd.movement.y *= 0.3;
		}
		if (CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).y == 1) {
			rgd.beforeLanding = 3;
		}
		hittedBlocks.erase(hittedBlocks.begin() + target);
	}
}

///<summary>
///<para>RigidBody�ɑ΂��ďd�́A��C��R�A�n�`�u���b�N�̏Փˏ������ꊇ�ōs��</para>
///<para>...</para>
///<para>rgd = �����߂����RigidBody</para>
///<para>gravity = ������d�͂�x,y����</para>
///<para>drag = x,y�����̃X�s�[�h�ێ����i�O�`�P�̏�������͂��Ăˁj</para>
///<para>blocks = rgd�������߂��n�`�u���b�N�̔z��</para>
/// </summary>
void RigidBodyBehaviour(RigidBody& rgd, Vector2D gravity, Vector2D drag, vector<GameObject> blocks) {
	GravityAndDrag(rgd, gravity, drag);
	float physicsLoop = max(rgd.movement.x, rgd.movement.y) / min(rgd.gameObject.entity.w, rgd.gameObject.entity.h) * 4;
	float physicsDiv = (max(rgd.movement.x, rgd.movement.y) - fmod(max(rgd.movement.x, rgd.movement.y), min(rgd.gameObject.entity.w, rgd.gameObject.entity.h))) / min(rgd.gameObject.entity.w, rgd.gameObject.entity.h) * 4;
	physicsDiv = max(1, physicsDiv);
	collideWall(rgd, blocks);
	for (int i = 0; i < physicsDiv; i++) {
		rgd.gameObject.beforePos = Vector2D{ rgd.gameObject.entity.x,rgd.gameObject.entity.y };
		rgd.gameObject.entity.x += rgd.movement.x / physicsDiv;
		rgd.gameObject.entity.y += rgd.movement.y / physicsDiv;
		collideWall(rgd, blocks);
	}
	if (rgd.beforeLanding > 0)
	{
		rgd.beforeLanding--;
		rgd.landing = true;
	}
	else
	{
		rgd.landing = false;
	}
	rgd.gameObject.beforeSca = Vector2D{ rgd.gameObject.entity.w,rgd.gameObject.entity.h };
}

///<summary>
/// <para>��`�Ɖ~�̓����蔻��</para>
/// <para>�������ĂȂ��Ƃ��̔����S������āA�S��false�Ȃ瓖�����Ă锻��</para>
///<para>...</para>
/// <para>Left = ��`�̍���</para>
/// <para>Right = ��`�̉E��</para>
/// <para>Top = ��`�̏��</para>
/// <para>Bottom = ��`�̒�</para>
/// <para>x = �~��X���W</para>
/// <para>y = �~��Y���W</para>
/// <para>r = �~�̔��a</para>
/// </summary>
int CollisionBox2Circle(int Left, int Right, int Top, int Bottom, int x, int y, int r) {
	if (Left - r > x || Right + r < x || Top - r > y || Bottom + r < y) {												//��`�ɉ~�̔��a���𑫂����͈�
		return false;
	}
	if (Left > x && Top > y && !((Left - x) * (Left - x) + (Top - y) * (Top - y) < r * r)) {				//����̓����蔻��
		return false;
	}
	if (Right < x && Top > y && !((Right - x) * (Right - x) + (Top - y) * (Top - y) < r * r)) {				//�E��̓����蔻��
		return false;
	}
	if (Left > x && Bottom < y && !((Left - x) * (Left - x) + (Bottom - y) * (Bottom - y) < r * r)) {		//�����̓����蔻��
		return false;
	}
	if (Right < x && Bottom < y && !((Right - x) * (Right - x) + (Bottom - y) * (Bottom - y) < r * r)) {	//�E���̓����蔻��
		return false;
	}
	return true;//���ׂĂ̏������O�ꂽ�Ƃ��ɓ������Ă���
}