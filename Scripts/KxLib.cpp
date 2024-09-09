// -------------------------------------------------------------------------------
// 
// 		KXライブラリ		cppファイル
// 
// 				Ver 20240909.0
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

float VectorScale(Vector2D a, Vector2D b) {
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

Vector2D RadToPos(float rad) {
	Vector2D ret;
	ret.x = sin(rad);
	ret.y = -cos(rad);
	return ret;
}

float PosToRad(Vector2D nowPos, Vector2D targetPos) {
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
	DrawRotaGraph(obj.entity.position.x - scroll.x, obj.entity.position.y - scroll.y, obj.graphScale, obj.rot, obj.graphNum, true, obj.dir);
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
	return r.position.x + r.scale.x >= p.x && r.position.x - r.scale.x <= p.x
		&& r.position.y + r.scale.y >= p.y && r.position.y - r.scale.y <= p.y;
}

bool HitRectAndRect(Rect a, Rect b) {
	return a.position.x - a.scale.x <= b.position.x + b.scale.x
		&& a.position.x + a.scale.x >= b.position.x - b.scale.x
		&& a.position.y - a.scale.y <= b.position.y + b.scale.y
		&& a.position.y + a.scale.y >= b.position.y - b.scale.y;
}

Vector2D CollisionInfoRectAndRect(GameObject obj, Rect wall) {
	Vector2D ret = Vector2D{ 0,0 };
	obj.entity = Rect{ (float)obj.beforePos.x,(float)obj.beforePos.y,obj.entity.scale.x,obj.entity.scale.y };
	if (abs(wall.position.y - obj.entity.position.y) * (wall.scale.x + obj.beforeSca.x) < abs(wall.position.x - obj.entity.position.x) * (wall.scale.y + obj.beforeSca.y)) {
		if (obj.entity.position.x <= wall.position.x) {
			ret.x = 1;
		}
		else {
			ret.x = -1;
		}
	}
	else {
		if (obj.entity.position.y <= wall.position.y) {
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
		if (obj.entity.position.x <= wall.position.x) {
			obj.entity.position.x -= (obj.entity.position.x + obj.entity.scale.x) - (wall.position.x - wall.scale.x);
		}
		else {
			obj.entity.position.x -= (obj.entity.position.x - obj.entity.scale.x) - (wall.position.x + wall.scale.x);
		}
	}
	else {
		if (obj.entity.position.y <= wall.position.y) {
			obj.entity.position.y -= (obj.entity.position.y + obj.entity.scale.y) - (wall.position.y - wall.scale.y);
		}
		else {
			obj.entity.position.y -= (obj.entity.position.y - obj.entity.scale.y) - (wall.position.y + wall.scale.y);
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
				VectorScale(Vector2D{ rgd.gameObject.entity.position.x ,rgd.gameObject.entity.position.y }, Vector2D{ hittedBlocks[j].entity.position.x ,hittedBlocks[j].entity.position.y });
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
	float physicsLoop = max(rgd.movement.x, rgd.movement.y) / min(rgd.gameObject.entity.scale.x, rgd.gameObject.entity.scale.y) * 4;
	float physicsDiv = (max(rgd.movement.x, rgd.movement.y) - fmod(max(rgd.movement.x, rgd.movement.y), min(rgd.gameObject.entity.scale.x, rgd.gameObject.entity.scale.y))) / min(rgd.gameObject.entity.scale.x, rgd.gameObject.entity.scale.y) * 4;
	physicsDiv = max(1, physicsDiv);
	collideWall(rgd, blocks);
	for (int i = 0; i < physicsDiv; i++) {
		rgd.gameObject.beforePos = Vector2D{ rgd.gameObject.entity.position.x,rgd.gameObject.entity.position.y };
		rgd.gameObject.entity.position.x += rgd.movement.x / physicsDiv;
		rgd.gameObject.entity.position.y += rgd.movement.y / physicsDiv;
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
	rgd.gameObject.beforeSca = Vector2D{ rgd.gameObject.entity.scale.x,rgd.gameObject.entity.scale.y };
}

int HitRectAndCircle(Rect rect, int x, int y, int radius)
{
	int right = rect.position.x + rect.scale.x;
	int left = rect.position.x - rect.scale.x;
	int top = rect.position.y + rect.scale.y;
	int bottom = rect.position.y - rect.scale.y;

	if (left - radius > x || right + radius < x || top - radius > y || bottom + radius < y) {						//矩形に円の半径分を足した範囲
		return false;
	}
	if (left > x && top > y && !((left - x) * (left - x) + (top - y) * (top - y) < radius * radius)) {				//左上の当たり判定
		return false;
	}
	if (right < x && top > y && !((right - x) * (right - x) + (top - y) * (top - y) < radius * radius)) {			//右上の当たり判定
		return false;
	}
	if (left > x && bottom < y && !((left - x) * (left - x) + (bottom - y) * (bottom - y) < radius * radius)) {		//左下の当たり判定
		return false;
	}
	if (right < x && bottom < y && !((right - x) * (right - x) + (bottom - y) * (bottom - y) < radius * radius)) {	//右下の当たり判定
		return false;
	}
	return true;//すべての条件が外れたときに当たっている
}