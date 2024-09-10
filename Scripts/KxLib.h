// -------------------------------------------------------------------------------
// 
// 		KX���C�u����		�w�b�_�t�@�C��
// 
// 				Ver 20240909.0
// 
// -------------------------------------------------------------------------------

#pragma once
#include <vector>
using namespace std;

#define PI 3.1415926535897932384626433832795f

///<summary>
///�񎟌��x�N�g��
///<para>...</para>
///<para>x,y = �x�N�g����x����,y����</para>
///</summary>
struct Vector2D {
	float x = 0;
	float y = 0;
};

///<summary>
///<para>��`�̈ʒu�Ƒ傫��</para>
///<para>...</para>
///<para>x,y = ��`�̒��S���W</para>
///<para>w,h = ��`�̉���,����</para>
///</summary>
struct Rect
{
	Vector2D position = Vector2D{};
	Vector2D scale = Vector2D{};
};

///<summary>
///�摜�Ƌ�`�̏��̉�
///<para>...</para>
///<para>entity = ��`�̏��</para>
///<para>graphNum = �\��t����摜�̔ԍ�</para>
///<para>graphScale = �摜�̑傫��</para>
///<para>graphLocalPos = �摜�̑��΍��W</para>
///<para>dir = �摜�����E���]�����邩</para>
///<para>beforePos = �P�t���[���O�̈ʒu���L������ꏊ�A����������K�v�Ȃ�</para>
///<para>beforeSca = �P�t���[���O�̑傫�����L������ꏊ�A����������K�v�Ȃ�</para>
/// </summary>
struct GameObject {
	Rect entity;
	int graphNum;
	float graphScale = 1;
	Vector2D graphLocalPos = Vector2D{};
	float rot = 0;
	bool dir = false;
	Vector2D beforePos = entity.position;
	Vector2D beforeSca = entity.scale;
};

///<summary>
///���������̂��߂̏��
///<para>...</para>
///<para>gameObject = �摜�Ƌ�`�̏��̉�</para>
///<para>movement = �ړ����x</para>
///<para>landing = �������n�ʂɐG��Ă��邩�ۂ�</para>
///<para>beforeLanding = ���n����ۑ����邽�߂̕ϐ�</para>
/// </summary>
struct RigidBody {
	GameObject gameObject;
	Vector2D movement = Vector2D{};
	bool landing = false;
	int beforeLanding = 0;
};

///<summary>
///�Q�_�Ԃ̋���
///<para>...</para>
///<para>a,b = �Q�_�̍��W</para>
/// </summary>
float VectorScale(Vector2D a);

///<summary>
///�Q�_�Ԃ̃x�N�g���𐳋K��
///<para>...</para>
///<para>a,b = �Q�_�̍��W</para>
/// </summary>
Vector2D NormalizedVector(Vector2D a);

///<summary>
/// <para>�x�N�g�������Z</para>
/// </summary>
Vector2D operator +(const Vector2D& v0, const Vector2D& v1);

///<summary>
/// <para>�x�N�g�������Z</para>
/// </summary>
Vector2D operator -(const Vector2D& v0, const Vector2D& v1);

///<summary>
/// <para>�x�N�g���|���Z</para>
/// </summary>
Vector2D operator *(const Vector2D& v, const float& f);

///<summary>
/// <para>�x�N�g������Z</para>
/// </summary>
Vector2D operator /(const Vector2D& v, const float& f);

///<summary>
//�x���@�̊p�x�����W�A���p�ɕϊ�����
///<para>...</para>
///<para>degree = �x���@�̊p�x</para>
/// </summary>
float DegreeToRad(float degree);

///<summary>
//���W�A���p��x���@�̊p�x�ɕϊ�����
///<para>...</para>
///<para>rad = ���W�A���p</para>
/// </summary>
float RadToDegree(float rad);

///<summary>
///<para>���͂������W�A���p�̕����������������P�̃x�N�g�����o�͂���</para>
///<para>...</para>
///<para>rad = ���W�A���p</para>
/// </summary>
Vector2D RadToPos(float rad);

///<summary>
///<para>�������̍��W����������̍��W�����������̊p�x</para>
///<para>...</para>
///<para>nowPos = ��ƂȂ���W</para>
///<para>targetPos = �����Ώۂ̍��W</para>
/// </summary>
float PosToRad(Vector2D nowPos, Vector2D targetPos);

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
void SetNextScene(float& progress, int& nextscene, int setNextScene);

///<summary>
///<para>�V�[����؂�ւ��邽�߂̍X�V�����i�X�V���������̉����ɒu���j</para>
///<para>SetNextScene()�ƕ��p����</para>
///<para>...</para>
///<para>progress = �V�[���ύX�܂ł̎c�莞�Ԃ��Ǘ�����ϐ�</para>
///<para>scene = ���݂̃V�[�����Ǘ�����ϐ�</para>
///<para>nextScene = ���̃V�[�����i�[����ϐ�</para>
///</summary>
void SceneUpdate(float& progress, int& scene, int nextScene);

///<summary>
///GameObject��`�悷��
///<para>...</para>
///<para>obj = �摜�Ƌ�`�̏��̉�</para>
///<para>scroll = �J�����̈ʒu</para>
/// </summary>
void RenderObject(GameObject obj, Vector2D scroll = Vector2D{ 0,0 });

///<summary>
///�z����̑SGameObject��`�悷��
///<para>...</para>
///<para>obj = �`�悷��GameObject�̔z��</para>
///<para>scroll = �J�����̈ʒu</para>
/// </summary>
void RenderAllObject(vector<GameObject> obj, Vector2D scroll = Vector2D{ 0,0 });

///<summary>
///RigidBody�ɏd�͂Ƌ�C��R��������
///<para>...</para>
///<para>rgd = �Ώۂ�RigidBody</para>
///<para>gravity = ������d�͂�x,y����</para>
///<para>drag = x,y�����̃X�s�[�h�ێ����i�O�`�P�̏�������͂��Ăˁj</para>
/// </summary>
void GravityAndDrag(RigidBody& rgd, Vector2D gravity, Vector2D drag);

///<summary>
///�}�b�v�`�b�v����ɒn�`�u���b�N����ׂ�
///<para>...</para>
///<para>tileMap = �}�b�v�`�b�v���L���ꂽ�z��</para>
///<para>tileScale = ���������^�C���̑傫��</para>
///<para>tile = �n�`�u���b�N�ɓ\��t����摜�̔ԍ�</para>
///<para>blocks = �o���オ�����n�`�u���b�N���i�[����z��</para>
/// </summary>
void ConstructMap(vector<vector<int>> tileMap, const int tileScale, const int tile, vector<GameObject>& blocks, float graphScale = 1);

///<summary>
///��`���m���d�Ȃ��Ă��邩�ۂ�
///<para>...</para>
///<para>a,b = ������s���Q�̋�`</para>
/// </summary>
bool HitRectAndRect(Rect a, Rect b);
bool HitRectAndPoint(Rect r, Vector2D p);

///<summary>
///�ǂ̕������Ώۂ̋�`�ɓ������Ă��邩
///<para>...</para>
///<para>obj = ��������I�u�W�F�N�g</para>
///<para>wall =�G��Ă���n�`�u���b�N</para>
/// </summary>
Vector2D CollisionInfoRectAndRect(GameObject obj, Rect wall);
///<summary>
///�Ώۂ̋�`�ɑ΂��ĉ����߂�
///<para>...</para>
///<para>obj = �����߂����I�u�W�F�N�g</para>
///<para>wall = obj�������߂��n�`�u���b�N</para>
/// </summary>
void CollisionRectAndRect(GameObject& obj, Rect wall);

///<summary>
///RigidBody��z����̑S�Ă̒n�`�u���b�N�ɑ΂��ĉ����߂�
///<para>...</para>
///<para>rgd = �����߂����RigidBody</para>
///<para>blocks = rgd�������߂��n�`�u���b�N�̔z��</para>
/// </summary>
void collideWall(RigidBody& rgd, vector<GameObject> blocks = {});

///<summary>
///<para>RigidBody�ɑ΂��ďd�́A��C��R�A�n�`�u���b�N�̏Փˏ������ꊇ�ōs��</para>
///<para>...</para>
///<para>rgd = �����߂����RigidBody</para>
///<para>gravity = ������d�͂�x,y����</para>
///<para>drag = x,y�����̃X�s�[�h�ێ����i�O�`�P�̏�������͂��Ăˁj</para>
///<para>blocks = rgd�������߂��n�`�u���b�N�̔z��</para>
/// </summary>
void RigidBodyUpdate(RigidBody& rgd, Vector2D gravity, Vector2D drag, vector<GameObject> blocks = {});

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
bool HitRectAndCircle(Rect rect, int x, int y, int radius);