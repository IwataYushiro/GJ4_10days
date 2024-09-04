// -------------------------------------------------------------------------------
// 
// 		KXライブラリ		cppファイル
// 
// 				Ver 20211115.0
// 
// -------------------------------------------------------------------------------

#include "KxLib.h"
#include "DxLib.h"
#include <math.h>

using namespace std;

///<summary>
///２点間の距離
///<para>...</para>
///<para>a,b = ２点の座標</para>
/// </summary>
double VectorScale(Vector2D a, Vector2D b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

///<summary>
///２点間のベクトルを正規化
///<para>...</para>
///<para>a,b = ２点の座標</para>
/// </summary>
Vector2D NormalizedVector(Vector2D a, Vector2D b) {
	if (VectorScale(a, b) != 0) {
		return Vector2D{ (b.x - a.x) / VectorScale(a, b),(b.y - a.y) / VectorScale(a, b) };
	}
	return Vector2D{ 0,0 };
}

///<summary>
//度数法の角度をラジアン角に変換する
///<para>...</para>
///<para>degree = 度数法の角度</para>
/// </summary>
float DegreeToRad(float degree) {
	return degree * PI / 180;
}

///<summary>
//ラジアン角を度数法の角度に変換する
///<para>...</para>
///<para>rad = ラジアン角</para>
/// </summary>
float RadToDegree(float rad) {
	return rad / PI * 180;
}

///<summary>
///<para>入力したラジアン角の方向を向いた長さ１のベクトルを出力する</para>
///<para>...</para>
///<para>rad = ラジアン角</para>
/// </summary>
Vector2D RadToPos(double rad) {
	Vector2D ret;
	ret.x = sin(rad);
	ret.y = -cos(rad);
	return ret;
}

///<summary>
///<para>第一引数の座標から第二引数の座標を向いた時の角度</para>
///<para>...</para>
///<para>nowPos = 基準となる座標</para>
///<para>targetPos = 向く対象の座標</para>
/// </summary>
double PosToRad(Vector2D nowPos, Vector2D targetPos) {
	return atan2(targetPos.x - nowPos.x, nowPos.y - targetPos.y);
}

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
void SetNextScene(float& progress, int& nextscene, int setNextScene) {
	nextscene = setNextScene;
	if (progress >= 1) {
		progress = 0;
	}
}

///<summary>
///<para>シーンを切り替えるための更新処理（更新処理部分の下側に置く）</para>
///<para>SetNextScene()と併用する</para>
///<para>...</para>
///<para>progress = シーン変更までの残り時間を管理する変数</para>
///<para>scene = 現在のシーンを管理する変数</para>
///<para>nextScene = 次のシーンを格納する変数</para>
///</summary>
void SceneUpdate(float& progress, int& scene, int nextScene) {
	if (progress < 1) {
		progress += 0.01;
	}
}

///<summary>
///GameObjectを描画する
///<para>...</para>
///<para>obj = 画像と矩形の情報の塊</para>
///<para>scroll = カメラの位置</para>
/// </summary>
void RenderObject(GameObject obj, Vector2D scroll) {
	DrawRotaGraph(obj.entity.x - scroll.x, obj.entity.y + scroll.y, obj.graphScale, obj.rot, obj.graphNum, true, obj.dir);
}

///<summary>
///配列内の全GameObjectを描画する
///<para>...</para>
///<para>obj = 描画するGameObjectの配列</para>
///<para>scroll = カメラの位置</para>
/// </summary>
void RenderAllObject(vector<GameObject> obj, Vector2D scroll) {
	for (int i = 0; i < obj.size(); i++) {
		RenderObject(obj[i], scroll);
	}
}

///<summary>
///RigidBodyに重力と空気抵抗を加える
///<para>...</para>
///<para>rgd = 対象のRigidBody</para>
///<para>gravity = 加える重力のx,y成分</para>
///<para>drag = x,y方向のスピード維持率（０～１の小数を入力してね）</para>
/// </summary>
void GravityAndDrag(RigidBody& rgd, Vector2D gravity, Vector2D drag) {
	rgd.movement.x += gravity.x;
	rgd.movement.y += gravity.y;
	rgd.movement.x *= drag.x;
	rgd.movement.y *= drag.y;
}

///<summary>
///マップチップを基に地形ブロックを並べる
///<para>...</para>
///<para>tileMap = マップチップが記された配列</para>
///<para>tileScale = 生成されるタイルの大きさ</para>
///<para>tile = 地形ブロックに貼り付ける画像の番号</para>
///<para>blocks = 出来上がった地形ブロックを格納する配列</para>
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
///矩形同士が重なっているか否か
///<para>...</para>
///<para>a,b = 判定を行う２つの矩形</para>
/// </summary>
bool HitRectAndRect(rect a, rect b) {
	if (a.x - a.w * 0.5 <= b.x + b.w * 0.5 && a.x + a.w * 0.5 >= b.x - b.w * 0.5 && a.y - a.h * 0.5 <= b.y + b.h * 0.5 && a.y + a.h * 0.5 >= b.y - b.h * 0.5) {
		return true;
	}
	return false;
}

///<summary>
///どの部分が対象の矩形に当たっているか
///<para>...</para>
///<para>obj = 判定を取るオブジェクト</para>
///<para>wall =触れている地形ブロック</para>
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
///対象の矩形に対して押し戻す
///<para>...</para>
///<para>obj = 押し戻されるオブジェクト</para>
///<para>wall = objを押し戻す地形ブロック</para>
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
///RigidBodyを配列内の全ての地形ブロックに対して押し戻す
///<para>...</para>
///<para>rgd = 押し戻されるRigidBody</para>
///<para>blocks = rgdを押し戻す地形ブロックの配列</para>
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
///<para>RigidBodyに対して重力、空気抵抗、地形ブロックの衝突処理を一括で行う</para>
///<para>...</para>
///<para>rgd = 押し戻されるRigidBody</para>
///<para>gravity = 加える重力のx,y成分</para>
///<para>drag = x,y方向のスピード維持率（０～１の小数を入力してね）</para>
///<para>blocks = rgdを押し戻す地形ブロックの配列</para>
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
int CollisionBox2Circle(int Left, int Right, int Top, int Bottom, int x, int y, int r) {
	if (Left - r > x || Right + r < x || Top - r > y || Bottom + r < y) {												//矩形に円の半径分を足した範囲
		return false;
	}
	if (Left > x && Top > y && !((Left - x) * (Left - x) + (Top - y) * (Top - y) < r * r)) {				//左上の当たり判定
		return false;
	}
	if (Right < x && Top > y && !((Right - x) * (Right - x) + (Top - y) * (Top - y) < r * r)) {				//右上の当たり判定
		return false;
	}
	if (Left > x && Bottom < y && !((Left - x) * (Left - x) + (Bottom - y) * (Bottom - y) < r * r)) {		//左下の当たり判定
		return false;
	}
	if (Right < x && Bottom < y && !((Right - x) * (Right - x) + (Bottom - y) * (Bottom - y) < r * r)) {	//右下の当たり判定
		return false;
	}
	return true;//すべての条件が外れたときに当たっている
}