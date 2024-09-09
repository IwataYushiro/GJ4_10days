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

Vector2D operator +(const Vector2D& v0, const Vector2D& v1)
{
	return Vector2D{v0.x + v1.x,v0.y + v1.y};
}

Vector2D operator -(const Vector2D& v0, const Vector2D& v1)
{
	return Vector2D{ v0.x - v1.x,v0.y - v1.y };
}

double VectorScale(Vector2D a, Vector2D b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Vector2D NormalizedVector(Vector2D a, Vector2D b) {
	if (VectorScale(a, b) != 0) {
		return Vector2D{ (b.x - a.x) / VectorScale(a, b),(b.y - a.y) / VectorScale(a, b) };
	}
	return Vector2D{ 0,0 };
}

float DegreeToRad(float degree) {
	return degree * PI / 180;
}

float RadToDegree(float rad) {
	return rad / PI * 180;
}

Vector2D RadToPos(double rad) {
	Vector2D ret;
	ret.x = sin(rad);
	ret.y = -cos(rad);
	return ret;
}

double PosToRad(Vector2D nowPos, Vector2D targetPos) {
	return atan2(targetPos.x - nowPos.x, nowPos.y - targetPos.y);
}

void SetNextScene(float& progress, int& nextscene, int setNextScene) {
	nextscene = setNextScene;
	if (progress >= 1) {
		progress = 0;
	}
}

void SceneUpdate(float& progress, int& scene, int nextScene) {
	if (progress < 1) {
		progress += 0.01;
	}
}

void RenderObject(GameObject obj, Vector2D scroll) {
	DrawRotaGraph(obj.entity.x - scroll.x, obj.entity.y - scroll.y, obj.graphScale, obj.rot, obj.graphNum, true, obj.dir);
}

void RenderAllObject(vector<GameObject> obj, Vector2D scroll) {
	for (int i = 0; i < obj.size(); i++) {
		RenderObject(obj[i], scroll);
	}
}

void GravityAndDrag(RigidBody& rgd, Vector2D gravity, Vector2D drag) {
	rgd.movement.x += gravity.x;
	rgd.movement.y += gravity.y;
	rgd.movement.x *= drag.x;
	rgd.movement.y *= drag.y;
}

void ConstructMap(vector<vector<int>> tileMap, const int tileScale, const int tile, vector<GameObject>& blocks, float graphScale) {
	blocks = {};
	for (int i = 0; i < tileMap.size(); i++) {
		for (int j = 0; j < tileMap[i].size(); j++) {
			if (tileMap[i][j] != 0) {
				blocks.push_back(GameObject{ Rect{ j * (float)tileScale + (float)tileScale / 2, i * (float)tileScale + (float)tileScale / 2, (float)tileScale, (float)tileScale }, tile, graphScale });
			}
		}
	}
}

bool HitRectAndPoint(Rect r, Vector2D p)
{
	return r.x + r.w >= p.x && r.x - r.w <= p.x
		&& r.y + r.h >= p.y && r.y - r.h <= p.y;
}

bool HitRectAndRect(Rect a, Rect b) {
	return a.x - a.w <= b.x + b.w
		&& a.x + a.w >= b.x - b.w
		&& a.y - a.h <= b.y + b.h
		&& a.y + a.h >= b.y - b.h;
}

Vector2D CollisionInfoRectAndRect(GameObject obj, Rect wall) {
	Vector2D ret = Vector2D{ 0,0 };
	obj.entity = Rect{ (float)obj.beforePos.x,(float)obj.beforePos.y,obj.entity.w,obj.entity.h };
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

void CollisionRectAndRect(GameObject& obj, Rect wall) {
	if (CollisionInfoRectAndRect(obj, wall).x != 0) {
		if (obj.entity.x <= wall.x) {
			obj.entity.x -= (obj.entity.x + obj.entity.w) - (wall.x - wall.w);
		}
		else {
			obj.entity.x -= (obj.entity.x - obj.entity.w) - (wall.x + wall.w);
		}
	}
	else {
		if (obj.entity.y <= wall.y) {
			obj.entity.y -= (obj.entity.y + obj.entity.h) - (wall.y - wall.h);
		}
		else {
			obj.entity.y -= (obj.entity.y - obj.entity.h) - (wall.y + wall.h);
		}
	}
}

void collideWall(RigidBody& rgd, vector<GameObject> blocks) {
	vector<GameObject> hittedBlocks = {};
	vector<GameObject> hittedEdgeBlocks = {};
	for (int i = 0; i < blocks.size(); i++) {
		if (HitRectAndRect(rgd.gameObject.entity, blocks[i].entity)) {
			hittedBlocks.push_back(blocks[i]);
		}
	}
	while (hittedBlocks.size() > 0) {
		float dist = 0;
		int target = 0;
		for (int j = 0; j < hittedBlocks.size(); j++) {
			int currentDist =
				VectorScale(Vector2D{ rgd.gameObject.entity.x ,rgd.gameObject.entity.y }, Vector2D{ hittedBlocks[j].entity.x ,hittedBlocks[j].entity.y });
			if (currentDist < dist || j == 0) {
				dist = currentDist;
				target = j;
			}
		}
		if (HitRectAndRect(rgd.gameObject.entity, hittedBlocks[target].entity)) {
			CollisionRectAndRect(rgd.gameObject, hittedBlocks[target].entity);
			if ((CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).x == 1 && rgd.movement.x > 0)
				|| (CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).x == -1 && rgd.movement.x < 0)) {
				//rgd.movement.x *= 0.3;
			}
			if ((CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).y == 1 && rgd.movement.y > 0)
				|| (CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).y == -1 && rgd.movement.y < 0)) {
				rgd.movement.y *= 0.3;
			}
			if (CollisionInfoRectAndRect(rgd.gameObject, hittedBlocks[target].entity).y == 1) {
				rgd.beforeLanding = 1;
			}
		}
		hittedBlocks.erase(hittedBlocks.begin() + target);
	}
}

void RigidBodyUpdate(RigidBody& rgd, Vector2D gravity, Vector2D drag, vector<GameObject> blocks) {
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