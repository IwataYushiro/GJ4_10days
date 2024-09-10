// -------------------------------------------------------------------------------
// 
// 		KXライブラリ		ヘッダファイル
// 
// 				Ver 20240909.0
// 
// -------------------------------------------------------------------------------

#pragma once
#include <vector>
using namespace std;

#define PI 3.1415926535897932384626433832795f

///<summary>
///二次元ベクトル
///<para>...</para>
///<para>x,y = ベクトルのx成分,y成分</para>
///</summary>
struct Vector2D {
	float x = 0;
	float y = 0;
};

///<summary>
///<para>矩形の位置と大きさ</para>
///<para>...</para>
///<para>x,y = 矩形の中心座標</para>
///<para>w,h = 矩形の横幅,高さ</para>
///</summary>
struct Rect
{
	Vector2D position = Vector2D{};
	Vector2D scale = Vector2D{};
};

///<summary>
///画像と矩形の情報の塊
///<para>...</para>
///<para>entity = 矩形の情報</para>
///<para>graphNum = 貼り付ける画像の番号</para>
///<para>graphScale = 画像の大きさ</para>
///<para>graphLocalPos = 画像の相対座標</para>
///<para>dir = 画像を左右反転させるか</para>
///<para>beforePos = １フレーム前の位置を記憶する場所、初期化する必要なし</para>
///<para>beforeSca = １フレーム前の大きさを記憶する場所、初期化する必要なし</para>
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
///物理挙動のための情報
///<para>...</para>
///<para>gameObject = 画像と矩形の情報の塊</para>
///<para>movement = 移動速度</para>
///<para>landing = 下側が地面に触れているか否か</para>
///<para>beforeLanding = 着地情報を保存するための変数</para>
/// </summary>
struct RigidBody {
	GameObject gameObject;
	Vector2D movement = Vector2D{};
	bool landing = false;
	int beforeLanding = 0;
};

///<summary>
///２点間の距離
///<para>...</para>
///<para>a,b = ２点の座標</para>
/// </summary>
float VectorScale(Vector2D a);

///<summary>
///２点間のベクトルを正規化
///<para>...</para>
///<para>a,b = ２点の座標</para>
/// </summary>
Vector2D NormalizedVector(Vector2D a);

///<summary>
/// <para>ベクトル足し算</para>
/// </summary>
Vector2D operator +(const Vector2D& v0, const Vector2D& v1);

///<summary>
/// <para>ベクトル引き算</para>
/// </summary>
Vector2D operator -(const Vector2D& v0, const Vector2D& v1);

///<summary>
/// <para>ベクトル掛け算</para>
/// </summary>
Vector2D operator *(const Vector2D& v, const float& f);

///<summary>
/// <para>ベクトル割り算</para>
/// </summary>
Vector2D operator /(const Vector2D& v, const float& f);

///<summary>
//度数法の角度をラジアン角に変換する
///<para>...</para>
///<para>degree = 度数法の角度</para>
/// </summary>
float DegreeToRad(float degree);

///<summary>
//ラジアン角を度数法の角度に変換する
///<para>...</para>
///<para>rad = ラジアン角</para>
/// </summary>
float RadToDegree(float rad);

///<summary>
///<para>入力したラジアン角の方向を向いた長さ１のベクトルを出力する</para>
///<para>...</para>
///<para>rad = ラジアン角</para>
/// </summary>
Vector2D RadToPos(float rad);

///<summary>
///<para>第一引数の座標から第二引数の座標を向いた時の角度</para>
///<para>...</para>
///<para>nowPos = 基準となる座標</para>
///<para>targetPos = 向く対象の座標</para>
/// </summary>
float PosToRad(Vector2D nowPos, Vector2D targetPos);

///<summary>
///<para>シーンを切り替える準備（シーンを切り替える処理をする場所に置く）</para>
///<para>これを使う前に現在のシーンを管理する変数と</para>
///<para>次のシーンを格納する変数と</para>
///<para>シーン変更までの残り時間を管理する変数を用意する</para>
///<para>...</para>
///<para>progress = シーン変更までの残り時間を管理する変数</para>
///<para>nextScene = 次のシーンを格納する変数</para>
///<para>setNextScene = 次のシーンを表す数値を直接入力</para>
///</summary>
void SetNextScene(float& progress, int& nextscene, int setNextScene);

///<summary>
///<para>シーンを切り替えるための更新処理（更新処理部分の下側に置く）</para>
///<para>SetNextScene()と併用する</para>
///<para>...</para>
///<para>progress = シーン変更までの残り時間を管理する変数</para>
///<para>scene = 現在のシーンを管理する変数</para>
///<para>nextScene = 次のシーンを格納する変数</para>
///</summary>
void SceneUpdate(float& progress, int& scene, int nextScene);

///<summary>
///GameObjectを描画する
///<para>...</para>
///<para>obj = 画像と矩形の情報の塊</para>
///<para>scroll = カメラの位置</para>
/// </summary>
void RenderObject(GameObject obj, Vector2D scroll = Vector2D{ 0,0 });

///<summary>
///配列内の全GameObjectを描画する
///<para>...</para>
///<para>obj = 描画するGameObjectの配列</para>
///<para>scroll = カメラの位置</para>
/// </summary>
void RenderAllObject(vector<GameObject> obj, Vector2D scroll = Vector2D{ 0,0 });

///<summary>
///RigidBodyに重力と空気抵抗を加える
///<para>...</para>
///<para>rgd = 対象のRigidBody</para>
///<para>gravity = 加える重力のx,y成分</para>
///<para>drag = x,y方向のスピード維持率（０～１の小数を入力してね）</para>
/// </summary>
void GravityAndDrag(RigidBody& rgd, Vector2D gravity, Vector2D drag);

///<summary>
///マップチップを基に地形ブロックを並べる
///<para>...</para>
///<para>tileMap = マップチップが記された配列</para>
///<para>tileScale = 生成されるタイルの大きさ</para>
///<para>tile = 地形ブロックに貼り付ける画像の番号</para>
///<para>blocks = 出来上がった地形ブロックを格納する配列</para>
/// </summary>
void ConstructMap(vector<vector<int>> tileMap, const int tileScale, const int tile, vector<GameObject>& blocks, float graphScale = 1);

///<summary>
///矩形同士が重なっているか否か
///<para>...</para>
///<para>a,b = 判定を行う２つの矩形</para>
/// </summary>
bool HitRectAndRect(Rect a, Rect b);
bool HitRectAndPoint(Rect r, Vector2D p);

///<summary>
///どの部分が対象の矩形に当たっているか
///<para>...</para>
///<para>obj = 判定を取るオブジェクト</para>
///<para>wall =触れている地形ブロック</para>
/// </summary>
Vector2D CollisionInfoRectAndRect(GameObject obj, Rect wall);
///<summary>
///対象の矩形に対して押し戻す
///<para>...</para>
///<para>obj = 押し戻されるオブジェクト</para>
///<para>wall = objを押し戻す地形ブロック</para>
/// </summary>
void CollisionRectAndRect(GameObject& obj, Rect wall);

///<summary>
///RigidBodyを配列内の全ての地形ブロックに対して押し戻す
///<para>...</para>
///<para>rgd = 押し戻されるRigidBody</para>
///<para>blocks = rgdを押し戻す地形ブロックの配列</para>
/// </summary>
void collideWall(RigidBody& rgd, vector<GameObject> blocks = {});

///<summary>
///<para>RigidBodyに対して重力、空気抵抗、地形ブロックの衝突処理を一括で行う</para>
///<para>...</para>
///<para>rgd = 押し戻されるRigidBody</para>
///<para>gravity = 加える重力のx,y成分</para>
///<para>drag = x,y方向のスピード維持率（０～１の小数を入力してね）</para>
///<para>blocks = rgdを押し戻す地形ブロックの配列</para>
/// </summary>
void RigidBodyUpdate(RigidBody& rgd, Vector2D gravity, Vector2D drag, vector<GameObject> blocks = {});

///<summary>
/// <para>矩形と円の当たり判定</para>
/// <para>当たってないとこの判定を全部取って、全部falseなら当たってる判定</para>
///<para>...</para>
/// <para>Left = 矩形の左辺</para>
/// <para>Right = 矩形の右辺</para>
/// <para>Top = 矩形の上辺</para>
/// <para>Bottom = 矩形の底</para>
/// <para>x = 円のX座標</para>
/// <para>y = 円のY座標</para>
/// <para>r = 円の半径</para>
/// </summary>
bool HitRectAndCircle(Rect rect, int x, int y, int radius);