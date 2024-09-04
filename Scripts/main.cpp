#include "DxLib.h"
#include "KxLib.h"
#include <math.h>
#include <time.h>
#include <vector>
#include <string>

using namespace std;

const char kWindowTitle[] = "GJ202409";

const float pi = 3.14159f;

// ウィンドウ横幅
const int WIN_WIDTH = 1000;

// ウィンドウ縦幅
const int WIN_HEIGHT = 800;


struct GameObject {
	Vector2D position;
	double rotation;
	double scale;
	int graphNum;
	int graphScale;
	bool arrive = true;
};

struct UserData {
	Vector2D mouse = { 0, 0 };
	Vector2D pin = { 0, 0 };
	bool clicked = false;
};

struct WiperBeam {
	UserData* userData = nullptr;
	Vector2D preMouse = { 0, 0 };
	int circleScale = 5;
	int waveScale = 999999;
	bool preClicked = false;
	bool circleExpand = false;
};

struct Playable {
	int life;
	int healWait;
};

struct noseAlien {
	GameObject gameObject;
	Playable status;
	int linePush;
	int vine;
};

struct DamageObject;

struct Enemy {
	GameObject gameObject;
	Playable status;
	int maxLife;
	int noseHit[2] = { 0 };
	int type = 0;
	int actionProgress = 0;
	int animationProgress = 0;
};

struct Spawner {
	Enemy enemy;
	int lifeTime = 100;
};

struct DamageObject {
	GameObject gameObject;
	Enemy parent;
};

struct Button {
	GameObject gameObject;
	Vector2D rect;
	string text;
	string eText;
	bool toggleEnabled;
	bool toggleStatus;
	int status;
};

struct Shard {
	GameObject gameObject;
	Vector2D move;
	float deceleration;
	int lifetime;
};

struct StageData
{
	bool battling = false;
	int stage = 0;
	int stageClearWait;
	int gameoverWait;
	int tutorialProgress = 0;
};

struct HostStageData
{
	vector<noseAlien> napnose = {};
	vector<noseAlien> MEGNOSE = {};
	vector<Enemy> enemy = {};
	vector<Shard> shards = {};
	vector<Shard> explodes = {};
	vector<DamageObject> damageObject = {};
	vector<GameObject> hitMark = {};
	vector<GameObject> damageMark = {};
};

enum sceneCollection {
	TITLESCREEN,
	MAINMENU,
	SETTING,
	CREDITS,
	STAGESELECT,
	BATTLE,
	GAMEOVER
};

GameObject newGameObject(Vector2D pos, double rot, double sca, int render, int graphScale) {
	GameObject ret;
	ret.position = pos;
	ret.rotation = rot;
	ret.scale = sca;
	ret.graphNum = render;
	ret.graphScale = graphScale;
	ret.arrive = true;
	return ret;
}

noseAlien newNose(GameObject obj, int life, int healWait) {
	noseAlien ret;
	ret.gameObject = obj;
	ret.status.life = life;
	ret.status.healWait = healWait;
	ret.vine = 160;
	ret.linePush = 0;
	return ret;
}

Enemy newEnemy(GameObject obj, int life, int type) {
	Enemy ret;
	ret.gameObject = obj;
	ret.maxLife = life;
	ret.status.life = life;
	ret.status.healWait = 0;
	ret.type = type;
	return ret;
}

DamageObject newDamageObject(GameObject obj, Enemy& parent) {
	DamageObject ret;
	ret.gameObject = obj;
	ret.parent = parent;
	return ret;
}

Vector2D newVector2(double a, double b) {
	Vector2D ret;
	ret.x = a;
	ret.y = b;
	return ret;
}

// ベクトル引き算(a-b)
Vector2D sub_vector(const Vector2D& a, const Vector2D& b) {
	Vector2D ret;
	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	return ret;
}

Vector2D GetMousePositionToV2D() {
	int mouseX;
	int mouseY;
	GetMousePoint(&mouseX, &mouseY);
	return (newVector2(mouseX, mouseY));
}

double VectorScale(Vector2D a, Vector2D b) {
	double ret;
	ret = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	return ret;
}

bool hitTriAndPoint(Vector2D A, Vector2D B, Vector2D C, Vector2D P) {

	// 線上は中とみなします。
	// ABCが三角形かどうかのチェックは省略...
	if (A.x - B.x != 0 && A.y - B.y != 0) {
		Vector2D AB = sub_vector(B, A);
		Vector2D BP = sub_vector(P, B);

		Vector2D BC = sub_vector(C, B);
		Vector2D CP = sub_vector(P, C);

		Vector2D CA = sub_vector(A, C);
		Vector2D AP = sub_vector(P, A);

		// 外積    Z成分だけ計算すればよいです
		double c1 = AB.x * BP.y - AB.y * BP.x;
		double c2 = BC.x * CP.y - BC.y * CP.x;
		double c3 = CA.x * AP.y - CA.y * AP.x;

		if ((c1 >= 0 && c2 >= 0 && c3 >= 0) || (c1 <= 0 && c2 <= 0 && c3 <= 0)) {
			return true;
		}
	}
	return false;
}

bool hitCircle(GameObject objectA, GameObject objectB, double range) {
	double dist = VectorScale(objectA.position, objectB.position);
	if (dist <= (objectA.scale + objectB.scale) * range && objectA.arrive && objectB.arrive) {
		return true;
	}
	else {
		return false;
	}
}

Vector2D LocalPosition(double x, double y, GameObject obj) {
	Vector2D ret;
	ret = newVector2(
		obj.position.x +
		(cos(obj.rotation * pi / 180) * x + -sin(obj.rotation * pi / 180) * y) * obj.scale,
		obj.position.y +
		(cos(obj.rotation * pi / 180) * y + sin(obj.rotation * pi / 180) * x) * obj.scale);
	return ret;
}

Vector2D AveragePosition(Vector2D posA, float perA, Vector2D posB, float perB) {
	return newVector2(
		(posA.x * perA + posB.x * perB) / (perA + perB),
		(posA.y * perA + posB.y * perB) / (perA + perB));
}

void CollideCircle(GameObject& light, GameObject heavy) {
	if (light.position.x == heavy.position.x && light.position.y == heavy.position.y) {
		light.position = LocalPosition(0, 2, heavy);
	}
	else {
		light.position = newVector2(
			light.position.x + ((light.position.x - heavy.position.x) /
				VectorScale(light.position, heavy.position)),
			light.position.y + ((light.position.y - heavy.position.y) /
				VectorScale(light.position, heavy.position)));
	}
}

void RenderObject(GameObject obj) {
	DrawRotaGraph(
		(int)obj.position.x, (int)obj.position.y, obj.scale * obj.graphScale, pi / 180 * obj.rotation,
		obj.graphNum, TRUE);
}

void DrawGage(double x, double y, float fillAmount, GameObject obj) {
	if (fillAmount > 1) {
		fillAmount = 1;
	}
	else if (fillAmount < 0) {
		fillAmount = 0;
	}

	bool gageRed = false;
	if (fillAmount < 0.25) {
		gageRed = true;
	}

	DrawLine(
		(int)LocalPosition(x, y, obj).x, (int)LocalPosition(x, y, obj).y,
		(int)LocalPosition(-x + x * fillAmount * 2, y, obj).x,
		(int)LocalPosition(-x + x * fillAmount * 2, y, obj).y,
		GetColor(255, 255, 255 - gageRed * 255));
	DrawLine(
		(int)LocalPosition(x - x * (1 - fillAmount) * 2, y, obj).x,
		(int)LocalPosition(x - x * (1 - fillAmount) * 2, y, obj).y,
		(int)LocalPosition(-x, y, obj).x,
		(int)LocalPosition(-x, y, obj).y, GetColor(gageRed * 255, 0, !gageRed * 255));
	DrawPixel(
		(int)LocalPosition(x, y, obj).x, (int)LocalPosition(x, y, obj).y, GetColor(255, 255, 255));
	DrawPixel(
		(int)LocalPosition(-x, y, obj).x, (int)LocalPosition(-x, y, obj).y,
		GetColor(255, 255, 255));
}

void DrawWiperBeam(WiperBeam wiperBeam)
{
	if (wiperBeam.userData->clicked) {
		DrawTriangle(
			(int)wiperBeam.userData->pin.x, (int)wiperBeam.userData->pin.y,
			(int)wiperBeam.preMouse.x, (int)wiperBeam.preMouse.y, (int)wiperBeam.userData->mouse.x,
			(int)wiperBeam.userData->mouse.y,
			GetColor(155, 255, 100),
			TRUE);
		DrawLine(
			(int)wiperBeam.userData->pin.x, (int)wiperBeam.userData->pin.y,
			(int)wiperBeam.userData->mouse.x, (int)wiperBeam.userData->mouse.y,
			GetColor(0, 255, 0));
		if (wiperBeam.waveScale >= 0) {
			DrawCircle(
				(int)wiperBeam.userData->pin.x, (int)wiperBeam.userData->pin.y,
				(int)wiperBeam.waveScale,
				GetColor(255, 255, 0), FALSE);
		}
		else {
			DrawCircle(
				(int)wiperBeam.userData->pin.x, (int)wiperBeam.userData->pin.y,
				wiperBeam.circleScale,
				GetColor(255, 255, 0), TRUE);
		}
		DrawCircle(
			(int)wiperBeam.userData->mouse.x, (int)wiperBeam.userData->mouse.y,
			wiperBeam.circleScale,
			GetColor(255, 255, 0), TRUE);
	}
	else if (wiperBeam.waveScale < 150) {
		DrawCircle(
			(int)wiperBeam.userData->mouse.x, (int)wiperBeam.userData->mouse.y, wiperBeam.waveScale,
			GetColor(255, 255, 0), FALSE);
	}
}

Button newButton(GameObject obj, Vector2D rect, string text, string eText) {
	Button ret;
	ret.rect = rect;
	ret.gameObject = obj;
	ret.text = text;
	ret.eText = eText;
	ret.toggleEnabled = false;
	ret.toggleStatus = false;
	return ret;
}

Button newButton(GameObject obj, Vector2D rect, string text, string eText, bool toggle) {
	Button ret;
	ret.rect = rect;
	ret.gameObject = obj;
	ret.text = text;
	ret.eText = eText;
	ret.toggleEnabled = true;
	ret.toggleStatus = toggle;
	return ret;
}

void ButtonBehaviour(Button& button, WiperBeam wiperBeam, int pushSound) {

	if (button.gameObject.position.x + button.rect.x < wiperBeam.userData->mouse.x ||
		button.gameObject.position.x - button.rect.x > wiperBeam.userData->mouse.x ||
		button.gameObject.position.y + button.rect.y < wiperBeam.userData->mouse.y ||
		button.gameObject.position.y - button.rect.y > wiperBeam.userData->mouse.y) {
		button.status = 0;
		return;
	}
	else
	{
		if (wiperBeam.userData->clicked) {
			if (button.gameObject.position.x + button.rect.x >= wiperBeam.userData->pin.x &&
				button.gameObject.position.x - button.rect.x <= wiperBeam.userData->pin.x &&
				button.gameObject.position.y + button.rect.y >= wiperBeam.userData->pin.y &&
				button.gameObject.position.y - button.rect.y <= wiperBeam.userData->pin.y)
			{
				button.status = 2;
				return;
			}
		}
		else
		{
			if (wiperBeam.preClicked
				&& (button.gameObject.position.x + button.rect.x >= wiperBeam.userData->pin.x &&
					button.gameObject.position.x - button.rect.x <= wiperBeam.userData->pin.x &&
					button.gameObject.position.y + button.rect.y >= wiperBeam.userData->pin.y &&
					button.gameObject.position.y - button.rect.y <= wiperBeam.userData->pin.y))
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

void DrawButton(Button button) {
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
		(int)(button.gameObject.position.x + button.rect.x),
		(int)(button.gameObject.position.y + button.rect.y),
		(int)(button.gameObject.position.x - button.rect.x),
		(int)(button.gameObject.position.y - button.rect.y),
		GetColor(color[0], color[1], color[2]), TRUE);
	DrawLine(
		(int)(button.gameObject.position.x + button.rect.x - 1),
		(int)(button.gameObject.position.y - button.rect.y),
		(int)(button.gameObject.position.x - button.rect.x),
		(int)(button.gameObject.position.y - button.rect.y),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawLine(
		(int)(button.gameObject.position.x - button.rect.x),
		(int)(button.gameObject.position.y + button.rect.y - 1),
		(int)(button.gameObject.position.x - button.rect.x),
		(int)(button.gameObject.position.y - button.rect.y - 1),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawTriangle(
		(int)(button.gameObject.position.x - button.rect.x),
		(int)(button.gameObject.position.y + button.rect.y),
		(int)(button.gameObject.position.x + button.rect.x),
		(int)(button.gameObject.position.y - button.rect.y),
		(int)(button.gameObject.position.x + button.rect.x),
		(int)(button.gameObject.position.y + button.rect.y),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2), TRUE);
	if (button.toggleEnabled) {
		DrawBox(
			(int)(button.gameObject.position.x + button.rect.x),
			(int)(button.gameObject.position.y - button.rect.y / 2),
			(int)(button.gameObject.position.x + button.rect.x / 2),
			(int)(button.gameObject.position.y - button.rect.y), GetColor(100, 100, 100), TRUE);
		if (button.toggleStatus) {
			DrawBox(
				(int)(button.gameObject.position.x + button.rect.x / 4 * 3),
				(int)(button.gameObject.position.y - button.rect.y / 2),
				(int)(button.gameObject.position.x + button.rect.x / 2),
				(int)(button.gameObject.position.y - button.rect.y), GetColor(100, 255, 100), TRUE);
		}
		else {
			DrawBox(
				(int)(button.gameObject.position.x + button.rect.x / 4 * 3),
				(int)(button.gameObject.position.y - button.rect.y / 2),
				(int)(button.gameObject.position.x + button.rect.x),
				(int)(button.gameObject.position.y - button.rect.y), GetColor(150, 0, 0), TRUE);
		}
	}
	DrawString(
		button.gameObject.position.x - button.rect.x, button.gameObject.position.y - button.rect.y,
		button.eText.c_str(),
		GetColor((color[0] + 127) / 2, (color[1] + 127) / 2, (color[2] + 127) / 2));
	DrawString(
		button.gameObject.position.x, button.gameObject.position.y, button.text.c_str(),
		GetColor(0, 0, 0));
}

bool IsButtonClicked(vector<Button>& buttons, int buttonNum, int status)
{
	return buttons.size() > buttonNum && buttons[buttonNum].status == status;
}

int Animation(int* pages, int startPage, int endPage, int framePerPage, int& progress) {
	progress++;
	if (progress < 0 || progress < startPage * framePerPage ||
		progress >(endPage + 1) * framePerPage - 1) {
		progress = startPage * framePerPage;
	}
	return pages[progress / framePerPage];
}

void SetNextScene(float& progress, sceneCollection& nextScene, sceneCollection setScene) {
	nextScene = setScene;
	progress = 0;
}

void Damage(Playable& status, int damageSE) {
	PlaySoundMem(damageSE, DX_PLAYTYPE_BACK, true);
	status.life -= 5;
	status.healWait = 300;
}

Shard newShard(GameObject obj, Vector2D move, float deceleration, int lifetime) {
	Shard ret;
	ret.gameObject = obj;
	ret.move = move;
	ret.deceleration = deceleration;
	ret.lifetime = lifetime;
	return ret;
}

void LifeBehavior(Playable& status, int maxLife) {
	if (status.healWait <= 0) {
		status.life++;
	}
	else {
		status.healWait--;
	}
	if (status.life > maxLife) {
		status.life = maxLife;
	}
	if (status.life < 0) {
		status.life = 0;
	}
}

void DrawMegaGage(
	vector<noseAlien>& nosesA, vector<noseAlien>& nosesB, float maxNose, float maxNoseLife,
	vector<Enemy>& enemies) {
	if ((nosesA.size() <= 0 && nosesB.size() <= 0) || enemies.size() <= 0) {
		return;
	}
	float nosesVit = 0;
	float enemiesVit = 0;
	for (int i = 0; i < nosesA.size(); i++) {
		if (nosesA[i].status.life * 1.0f / ((maxNoseLife * maxNose) * 2.0f) >= 0) {
			nosesVit += nosesA[i].status.life * 1.0f / ((maxNoseLife * maxNose) * 2.0f);
		}
	}
	for (int i = 0; i < nosesB.size(); i++) {
		if (nosesB[i].status.life * 1.0f / ((maxNoseLife * maxNose) * 2.0) >= 0) {
			nosesVit += nosesB[i].status.life * 1.0f / ((maxNoseLife * maxNose) * 2.0f);
		}
	}
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i].status.life * 1.0f / ((enemies[i].maxLife * enemies.size()) * 1.0f) >= 0) {
			enemiesVit +=
				enemies[i].status.life * 1.0f / ((enemies[i].maxLife * enemies.size()) * 1.0f);
		}
	}
	DrawBox(150, 20, WIN_WIDTH - 150, 80, GetColor(255, 60, 0), TRUE);
	DrawBox(
		150, 20, (int)(150 + (WIN_WIDTH - 300) * (nosesVit / (enemiesVit + nosesVit))), 80,
		GetColor(0, 60, 255), TRUE);
	DrawBox(150, 25, WIN_WIDTH - 150, 35, GetColor(255, 255, 255), TRUE);
}

void SpreadShard(
	vector<Shard>& shards, int shardNum, Vector2D shardMove, float moveRand, float deceleration,
	float scale, float scaleRand, int lifetime, GameObject parent) {
	for (int j = 0; j < shardNum; j++) {
		shards.push_back(newShard(
			parent,
			newVector2(
				shardMove.x * (1 + fmodf((float)rand(), moveRand)),
				shardMove.y * (1 + fmodf((float)rand(), moveRand))),
			deceleration, lifetime));
		shards[shards.size() - 1].gameObject.scale =
			parent.scale * scale * (1 + fmodf((float)rand() * 1.0f, scaleRand));
		shards[shards.size() - 1].gameObject.rotation += j * (360 / shardNum) + rand() % 21 - 10;
	}
}

void ShardsBehaviour(vector<Shard>& shards) {
	for (int i = 0; i < shards.size(); i++) {
		shards[i].gameObject.position =
			LocalPosition(shards[i].move.x, shards[i].move.y, shards[i].gameObject);
		shards[i].move = newVector2(
			shards[i].move.x * shards[i].deceleration, shards[i].move.y * shards[i].deceleration);
		shards[i].lifetime--;
		if (shards[i].lifetime <= 3) {
			shards[i].gameObject.scale *= 0.7;
		}
		if (shards[i].lifetime <= -1) {
			shards.erase(shards.begin() + i);
		}
	}
}

void pushBackField(GameObject& obj) {
	if (obj.position.x < 0) {
		obj.position.x = 0;
	}
	if (obj.position.x > WIN_WIDTH) {
		obj.position.x = WIN_WIDTH;
	}
	if (obj.position.y < 0) {
		obj.position.y = 0;
	}
	if (obj.position.y > WIN_HEIGHT) {
		obj.position.y = WIN_HEIGHT;
	}
}

bool AttackAble(DamageObject atk) {
	return atk.parent.noseHit[0] == 0 && atk.parent.noseHit[1] == 0;
}

void WiperBeamUpdate(WiperBeam& wiperBeam)
{
	if (wiperBeam.circleExpand) {
		wiperBeam.circleScale++;
	}
	else {
		wiperBeam.circleScale--;
	}
	if (wiperBeam.circleScale >= 9) {
		wiperBeam.circleExpand = false;
	}
	if (wiperBeam.circleScale <= 4) {
		wiperBeam.circleExpand = true;
	}

	if (wiperBeam.userData->clicked) {
		if (!wiperBeam.preClicked) {
			wiperBeam.waveScale = 100;
		}
		if (wiperBeam.waveScale >= 0) {
			wiperBeam.waveScale -= 10;
			wiperBeam.circleScale = 0;
		}
		wiperBeam.preMouse = wiperBeam.userData->mouse;
	}
	else {
		if (wiperBeam.preClicked) {
			wiperBeam.waveScale = 0;
		}
		wiperBeam.waveScale += 15;
	}
	wiperBeam.preClicked = wiperBeam.userData->clicked;
}

void NoseMove(
	vector<noseAlien>& noses, int noseGraph, int maxNose, int maxLife, int damageSE, int deathSE,
	const vector<WiperBeam>& wiperBeams, vector<DamageObject>& damageObject,
	vector<GameObject>& damageMark, vector<noseAlien>& pairNoses,
	vector<Shard>& shards) {
	for (int i = 0; i < noses.size(); i++) {
		if (noses[i].status.life > 0) {
			noses[i].gameObject.rotation += 3;
			noses[i].gameObject.graphNum = noseGraph;
			noses[i].vine--;
			noses[i].linePush--;

			if (noses[i].vine < 20) {
				noses[i].gameObject.scale = 0.2f;
			}
			else {
				noses[i].gameObject.scale = 0.3f;
			}
			if (noses[i].vine < 0) {
				if (pairNoses.size() <= 0) {
					pairNoses.push_back(newNose(
						newGameObject(
							LocalPosition(0, 90, noses[i].gameObject), noses[i].gameObject.rotation,
							0.3f, noseGraph, 1),
						noses[i].status.life, noses[i].status.healWait));
				}
				else if (noses.size() < maxNose) {
					noses.push_back(newNose(
						newGameObject(
							LocalPosition(0, 90, noses[i].gameObject), noses[i].gameObject.rotation,
							0.3f, noseGraph, 1),
						noses[i].status.life, noses[i].status.healWait));
				}
				noses[i].vine = 80;
			}

			for (int j = 0; j < damageObject.size(); j++) {
				if (hitCircle(noses[i].gameObject, damageObject[j].gameObject, 60) &&
					AttackAble(damageObject[j])) {
					damageMark.push_back(noses[i].gameObject);
					Damage(noses[i].status, damageSE);
				}
			}

			LifeBehavior(noses[i].status, maxLife);

			for (auto& itr : wiperBeams)
			{
				if (itr.userData->clicked && hitTriAndPoint(
					itr.userData->pin, itr.userData->mouse,
					itr.preMouse,
					noses[i].gameObject.position)) {
					noses[i].linePush = 15;
					noses[i].vine++;
					noses[i].gameObject.rotation -= 12;
					noses[i].gameObject.position = newVector2(
						noses[i].gameObject.position.x + itr.userData->mouse.x - itr.preMouse.x,
						noses[i].gameObject.position.y + itr.userData->mouse.y - itr.preMouse.y);
				}
			}

			for (int ii = i + 1; ii < noses.size(); ii++) {
				if (hitCircle(noses[i].gameObject, noses[ii].gameObject, 30) &&
					noses[i].linePush <= 0 && noses[ii].linePush <= 0 &&
					noses[ii].status.life > 0) {
					if (noses[i].status.life < noses[ii].status.life) {
						noses[i].gameObject.arrive = false;
						SpreadShard(
							shards, 4, newVector2(0, 45), 0.11f, 0.85f, 0.5f, 0.5f, 15,
							noses[i].gameObject);
					}
					else {
						noses[ii].gameObject.arrive = false;
						SpreadShard(
							shards, 4, newVector2(0, 45), 0.11f, 0.85f, 0.5f, 0.5f, 15,
							noses[ii].gameObject);
					}
				}
				else if (hitCircle(noses[i].gameObject, noses[ii].gameObject, 60)) {
					if (noses[i].linePush <= 0 && noses[ii].linePush <= 0) {
						CollideCircle(noses[i].gameObject, noses[ii].gameObject);
						CollideCircle(noses[ii].gameObject, noses[i].gameObject);
					}
				}
			}
			if (noses[i].status.life <= 0) {
				noses[i].status.life = 0;
				PlaySoundMem(deathSE, DX_PLAYTYPE_BACK, true);
			}
		}
		else {
			if (noses[i].status.life < -50) {
				SpreadShard(
					shards, 7, newVector2(0, 45), 0.11f, 0.85f, 0.5f, 0.6f, 20, noses[i].gameObject);
				noses[i].gameObject.arrive = false;
			}
			else {
				SpreadShard(
					shards, 1, newVector2(0, 85), 0.21f, 0.85f, 0.2f, 0.49f, 10, noses[i].gameObject);
				noses[i].status.life--;
				noses[i].gameObject.rotation += 51;
				noses[i].gameObject.position =
					LocalPosition(rand() % 101 - 50, rand() % 101 - 50, noses[i].gameObject);
			}
		}

		pushBackField(noses[i].gameObject);

		if (!noses[i].gameObject.arrive) {
			noses.erase(noses.begin() + i);
		}
	}
}

UserData currentPlayer;
UserData otherPlayer;
UserData otherPlayerLocal;

StageData stageData;
HostStageData hostStageData;


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	WSADATA wdData;
	static HANDLE hThread;
	static DWORD dwID;

	// ウィンドウモードに設定
	ChangeWindowMode(TRUE);

	// ウィンドウサイズを手動では変更させず、
	// かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// タイトルを変更
	SetMainWindowText(kWindowTitle);

	// 画面サイズの最大サイズ、カラービット数を設定(モニターの解像度に合わせる)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// 画面サイズを設定(解像度との比率で設定)
	SetWindowSizeExtendRate(1.0);

	// 画面の背景色を設定する
	SetBackgroundColor(80, 120, 250);

	//バックグラウンド状態でも停止しないよう設定
	SetAlwaysRunFlag(true);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// フォントのサイズを設定する
	SetFontSize(24);

	// 画像などのリソースデータの変数宣言と読み込み


	// ゲームループで使う変数の宣言
	WiperBeam currentWiper;
	currentWiper.userData = &currentPlayer;

	WiperBeam otherWiper;
	otherWiper.userData = &otherPlayerLocal;

	sceneCollection scene = TITLESCREEN;
	sceneCollection nextScene = TITLESCREEN;
	float cutSceneProgress = 1;
	bool cutInNose = false;

	int audioClip[] = { LoadSoundMem("Resources/BGM/Let'sOpenTheGate!.mp3"), LoadSoundMem("Resources/BGM/WoodsPolkka.mp3"),
		LoadSoundMem("Resources/BGM/MessedUpMayhem!.mp3"), LoadSoundMem("Resources/BGM/TapToReturnToTheTitleScreen.mp3"),
		LoadSoundMem("Resources/BGM/CityExplorer.mp3"), LoadSoundMem("Resources/BGM/Lowpolyism.mp3"),
		LoadSoundMem("Resources/BGM/BLOCCCCK!!.mp3"), LoadSoundMem("Resources/BGM/HeatsUpCombat!.mp3"),
		LoadSoundMem("Resources/BGM/H.B.C.S..mp3"), LoadSoundMem("Resources/BGM/ResistanceOrTrash.mp3") };
	int seClip[] = { LoadSoundMem("Resources/SE/SE_DAMAGE.wav"), LoadSoundMem("Resources/SE/hit.wav"),
		LoadSoundMem("Resources/SE/boom.wav"),LoadSoundMem("Resources/SE/kaboooooom.wav"),
		LoadSoundMem("Resources/SE/buttonPush.wav"),LoadSoundMem("Resources/SE/death.wav") };
	int bgmNum = 0;
	bool bgmSwitch = true;
	bool seSwitch = true;

	bool battleStarted = false;
	bool pause;
	int title = LoadGraph("Resources/Textures/nosecell_title.png");
	int clearGraph = LoadGraph("Resources/Textures/clear.png");
	int gameoverGraph = LoadGraph("Resources/Textures/gameover.png");
	int backGround = 0;
	int menuBG[4];
	LoadDivGraph("Resources/Textures/bg_menu.png", 4, 2, 2, 256, 256, menuBG);
	int battleBG[4];
	LoadDivGraph("Resources/Textures/bg_battle.png", 4, 2, 2, 256, 256, battleBG);
	GameObject tutorialWindow;
	int tutorialMov[19];
	LoadDivGraph("Resources/Textures/tutorial.png", 19, 8, 3, 128, 128, tutorialMov);
	int tutorialAnimeProgress;
	GameObject tutorialFlag;
	int flagMov[4];
	LoadDivGraph("Resources/Textures/flag.png", 4, 2, 2, 64, 64, flagMov);
	int flagAnimeProgress;
	bool flagTouched = false;
	int gameoverTipsNum = 999;
	string gameoverTipsText[] =
	{ "鼻に触れている敵が出す攻撃は灰色になり、\n当たってもダメージを受けなくなります。\n広範囲に及ぶ攻撃をしてきたらすかさず\n敵に鼻をぶつけて打ち消しましょう。",
	"新しく生成された鼻は分裂するまでに長い時間を要します。\nなるべく古い鼻を生き残らせておくことで、\n分裂速度を上げることができます。",
	"鼻が密集していると、新しく生成された鼻が\nすぐに別の鼻に吸収されてしまい、\nうまく増殖できません。ある程度増えてきたら\n塊の一部を動かして、空きスペースを作ってあげましょう。",
	"あまり１か所にまとめておくと、その部分を敵に\n攻撃されたとき一瞬で壊滅してしまいます。\nなるべくフィールド全体にまんべんなく\nバラけさせておきましょう。",
	"増殖するための鼻と、攻撃に使う鼻で分担するように\n立ち回ると、全滅のリスクを下げることができます。" };
	int telopPos;

	vector<Button> buttons;
	vector<Button> preButtons;
	const int maxNose = 70;
	const int maxNoseLife = 150;
	int noseGraph[2] = { LoadGraph("Resources/Textures/napnose.png"), LoadGraph("Resources/Textures/MEGNOSE.png") };
	int sandBagFakeNoseGraph[4];
	LoadDivGraph("Resources/Textures/sandBagFakeNose.png", 4, 2, 2, 64, 64, sandBagFakeNoseGraph);
	int FakeNoseRobotGraph[12];
	LoadDivGraph("Resources/Textures/fakeNoseRobot.png", 12, 4, 3, 64, 64, FakeNoseRobotGraph);
	int FakeNoseTankGraph[9];
	LoadDivGraph("Resources/Textures/fakeNoseTank.png", 9, 4, 3, 64, 64, FakeNoseTankGraph);
	int FakeNoseRoidGraph[12];
	LoadDivGraph("Resources/Textures/fakeNoseRoid.png", 12, 4, 3, 64, 64, FakeNoseRoidGraph);
	int FakeNoseDrillGraph[12];
	LoadDivGraph("Resources/Textures/fakeNoseDrill.png", 12, 4, 3, 64, 64, FakeNoseDrillGraph);
	int FakeNoseEmperorGraph[13];
	LoadDivGraph("Resources/Textures/fakeNoseEmperor.png", 13, 4, 4, 64, 64, FakeNoseEmperorGraph);

	bool gamePlay = true;

	// 最新のキーボード情報用
	char keys[256] = { 0 };

	// 1ループ(フレーム)前のキーボード情報
	char oldkeys[256] = { 0 };

	// ゲームループ
	while (gamePlay)
	{
		// 最新のキーボード情報だったものは1フレーム前のキーボード情報として保存
		// 最新のキーボード情報を取得
		GetHitKeyStateAll(keys);

		// 画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		// 更新処理
		hostStageData.damageObject = {};
		hostStageData.hitMark = {};
		hostStageData.damageMark = {};
		backGround = 0;

		if (scene != BATTLE) {
			hostStageData.napnose = {};
			hostStageData.MEGNOSE = {};
			hostStageData.enemy = {};
			hostStageData.explodes = {};
			hostStageData.shards = {};
			stageData.tutorialProgress = 0;
			stageData.stageClearWait = 0;
			stageData.gameoverWait = 0;
			pause = false;
			battleStarted = false;
			flagTouched = false;
			tutorialWindow = {};
			tutorialFlag = {};
		}

		stageData.battling = scene == BATTLE;

		switch (scene) {
		case TITLESCREEN:
			bgmNum = 0;
			if (!currentWiper.userData->clicked && currentWiper.preClicked) {
				PlaySoundMem(seClip[4], DX_PLAYTYPE_BACK, true);
				SetNextScene(cutSceneProgress, nextScene, MAINMENU);
			}
			backGround = menuBG[0];
			break;
		case MAINMENU:
			bgmNum = 1;
			preButtons = {
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 2 - 30, WIN_HEIGHT / 2 - 60), 0, 1, 0, 1),
					newVector2(200, 50), "メイン\n　ゲーム", "MAIN GAME"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 2 + 50), 0, 1, 0, 1),
					newVector2(200, 50), "オプション", "OPTION"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 2 + 30, WIN_HEIGHT / 2 + 160), 0, 1, 0, 1),
					newVector2(200, 50), "タイトルへ\n　もどる", "RETURN TITLE"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 2 + 60, WIN_HEIGHT / 2 + 270), 0, 1, 0, 1),
					newVector2(200, 50), "ゲームを\n　やめる", "QUIT GAME"),
			};
			if (IsButtonClicked(buttons, 0, 3)) {
				SetNextScene(cutSceneProgress, nextScene, STAGESELECT);
			}
			if (IsButtonClicked(buttons, 1, 3)) {
				SetNextScene(cutSceneProgress, nextScene, SETTING);
			}
			if (IsButtonClicked(buttons, 2, 3)) {
				SetNextScene(cutSceneProgress, nextScene, TITLESCREEN);
			}
			if (IsButtonClicked(buttons, 3, 3)) {
				gamePlay = false;
			}
			for (int i = 0; i < buttons.size(); i++) {
				if (!IsButtonClicked(buttons, i, 0)) {
					backGround = menuBG[i];
				}
			}
			break;
		case SETTING:
			bgmNum = 1;
			preButtons = {
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 3 * 2, WIN_HEIGHT / 2 - 200), 0, 1, 0, 1),
					newVector2(400, 50), "おんがく", "BGM", bgmSwitch),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 3 * 2, WIN_HEIGHT / 2 - 90), 0, 1, 0, 1),
					newVector2(400, 50), "こうかおん", "SE", seSwitch),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 3 * 2, WIN_HEIGHT / 2 + 20), 0, 1, 0, 1),
					newVector2(400, 50), "クレジット", "CREDITS"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 6 * 4, WIN_HEIGHT / 2 + 260), 0, 1, 0, 1),
					newVector2(300, 50), "もどる", "RETURN MENU"),
			};
			if (IsButtonClicked(buttons, 0, 3)) {
				bgmSwitch = !bgmSwitch;
			}
			if (IsButtonClicked(buttons, 1, 3)) {
				seSwitch = !seSwitch;
			}
			if (IsButtonClicked(buttons, 2, 3)) {
				SetNextScene(cutSceneProgress, nextScene, CREDITS);
			}
			if (IsButtonClicked(buttons, 3, 3)) {
				SetNextScene(cutSceneProgress, nextScene, MAINMENU);
			}
			backGround = menuBG[1];
			break;
		case CREDITS:
			bgmNum = 1;
			preButtons = {
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 6 * 4, WIN_HEIGHT / 2 + 260), 0, 1, 0, 1),
					newVector2(300, 50), "オプションへ\n　もどる", "RETURN SETTING"),
			};
			if (IsButtonClicked(buttons, 0, 3)) {
				SetNextScene(cutSceneProgress, nextScene, SETTING);
			}
			break;
		case STAGESELECT:
			bgmNum = 2;
			preButtons = {
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 7, WIN_HEIGHT / 10 * 9), 0, 1, 0, 1),
					newVector2(150, 50), "メイン\n　メニューへ", "RETURN MAINMENU"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 6, WIN_HEIGHT / 3), 0, 1, 0, 1),
					newVector2(180, 80), "ステージ\n　1\n　　（ﾁｭｰﾄﾘｱﾙ）", "STAGE 1\n TUTORIAL"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 3), 0, 1, 0, 1),
					newVector2(180, 80), "ステージ\n　2\n", "STAGE 2"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 6 * 5, WIN_HEIGHT / 3), 0, 1, 0, 1),
					newVector2(180, 80), "ステージ\n　3", "STAGE 3"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 6, WIN_HEIGHT / 5 * 3), 0, 1, 0, 1),
					newVector2(180, 80), "ステージ\n　4", "STAGE 4"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 5 * 3), 0, 1, 0, 1),
					newVector2(180, 80), "ステージ\n　5\n", "STAGE 5"),
				newButton(
					newGameObject(newVector2(WIN_WIDTH / 6 * 5, WIN_HEIGHT / 5 * 3), 0, 1, 0, 1),
					newVector2(180, 80), "ステージ\n　6", "STAGE 6"),
			};

			if (IsButtonClicked(buttons, 0, 3)) {
				SetNextScene(cutSceneProgress, nextScene, MAINMENU);
			}
			for (int i = 1; i < buttons.size(); i++) {
				if (IsButtonClicked(buttons, i, 3)) {
					stageData.stage = i;
					SetNextScene(cutSceneProgress, nextScene, BATTLE);
				}
			}
			backGround = menuBG[0];
			break;
		case BATTLE:
			switch (stageData.stage) {
			default:
				bgmNum = 4;
				backGround = battleBG[0];
				break;
			case 2:
				bgmNum = 5;
				backGround = battleBG[1];
				break;
			case 3:
				bgmNum = 6;
				backGround = battleBG[1];
				break;
			case 4:
				bgmNum = 7;
				backGround = battleBG[1];
				break;
			case 5:
				bgmNum = 8;
				backGround = battleBG[2];
				break;
			case 6:
				bgmNum = 9;
				backGround = battleBG[3];
				break;
			}

			if (pause) {
				preButtons = {
					newButton(
						newGameObject(newVector2(WIN_WIDTH / 4, WIN_HEIGHT / 4 * 3), 0, 1, 0, 1),
						newVector2(250, 50), "つづける", "CONTINUE"),
					newButton(
						newGameObject(
							newVector2(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 4 * 3), 0, 1, 0, 1),
						newVector2(250, 50), "ステージセレクトへ\n　もどる",
						"RETURN\n STAGE SELECT"),
				};
				if (IsButtonClicked(buttons, 0, 3)) {
					pause = false;
				}
				if (IsButtonClicked(buttons, 1, 3)) {
					SetNextScene(cutSceneProgress, nextScene, STAGESELECT);
				}
			}
			else {
				if (stageData.stageClearWait < 10 && stageData.gameoverWait <= 0) {
					preButtons = {
						newButton(
							newGameObject(newVector2(80, 60), 0, 1, 0, 1), newVector2(60, 40),
							"ﾎﾟｰｽﾞ", "PAUSE"),
					};
					if (IsButtonClicked(buttons, 0, 3)) {
						pause = true;
					}
				}

				if (!battleStarted) {
					hostStageData.napnose = { newNose(
						newGameObject(
							newVector2(WIN_WIDTH / 2 - 100, WIN_HEIGHT / 4 * 3), 0, 0.3f,
							noseGraph[0], 1),
						maxNoseLife, 0) };
					hostStageData.MEGNOSE = { newNose(
						newGameObject(
							newVector2(WIN_WIDTH / 2 + 100, WIN_HEIGHT / 4 * 3), 0, 0.3f,
							noseGraph[1], 1),
						maxNoseLife, 0) };
					switch (stageData.stage) {
					case 2:
						hostStageData.enemy = { newEnemy(
							newGameObject(
								newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 2), 0, 1, noseGraph[0], 1),
							60000, 2) };
						break;
					case 3:
						hostStageData.enemy = { newEnemy(
							newGameObject(
								newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 2), 0, 1, noseGraph[0], 1),
							90000, 3) };
						break;
					case 4:
						hostStageData.enemy = { newEnemy(
							newGameObject(
								newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 2), 0, 1, noseGraph[0], 1),
							120000, 4) };
						break;
					case 5:
						hostStageData.enemy = { newEnemy(
							newGameObject(
								newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 2), 0, 1, noseGraph[0], 1),
							240000, 5) };
						break;
					case 6:
						hostStageData.enemy = { newEnemy(
							newGameObject(
								newVector2(WIN_WIDTH / 2, WIN_HEIGHT / 2), 0, 1, noseGraph[0], 1),
							300000, 6) };
						break;
					}
					battleStarted = true;
				}

				if (stageData.stage == 1) {
					switch (stageData.tutorialProgress) {
					case 0:
						tutorialWindow = newGameObject(
							newVector2(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 4 * 3), 0, 3,
							Animation(tutorialMov, 0, 5, 15, tutorialAnimeProgress), 1);
						tutorialFlag = newGameObject(
							newVector2(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 4), 0, 2,
							Animation(flagMov, 0, 3, 5, flagAnimeProgress), 1);
						for (int i = 0; i < hostStageData.napnose.size(); i++) {
							if (hitCircle(tutorialFlag, hostStageData.napnose[i].gameObject, 30)) {
								flagTouched = true;
							}
						}
						for (int i = 0; i < hostStageData.MEGNOSE.size(); i++) {
							if (hitCircle(tutorialFlag, hostStageData.MEGNOSE[i].gameObject, 30)) {
								flagTouched = true;
							}
						}
						if (flagTouched) {
							hostStageData.enemy = { newEnemy(
								newGameObject(
									newVector2(WIN_WIDTH / 4, WIN_HEIGHT / 4), 0, 1, noseGraph[0],
									1),
								1000, 0) };
							stageData.tutorialProgress = 1;
						}
						break;
					case 1:
						tutorialWindow = newGameObject(
							newVector2(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 4 * 3), 0, 3,
							Animation(tutorialMov, 6, 16, 15, tutorialAnimeProgress), 1);
						tutorialFlag = {};
						if (hostStageData.enemy.size() <= 0) {
							hostStageData.enemy = { newEnemy(
								newGameObject(
									newVector2(WIN_WIDTH / 4, WIN_HEIGHT / 4 * 3), 0, 1,
									noseGraph[0], 1),
								40000, 1) };
							stageData.tutorialProgress = 2;
						}
						break;
					case 2:
						tutorialWindow = newGameObject(
							newVector2(WIN_WIDTH / 4 * 3, WIN_HEIGHT / 4 * 3), 0, 3,
							Animation(tutorialMov, 17, 18, 15, tutorialAnimeProgress), 1);
						tutorialFlag = {};
						if (hostStageData.enemy.size() <= 0) {
							stageData.stageClearWait++;
						}
						break;
					}
				}
				else {
					if (hostStageData.enemy.size() <= 0) {
						stageData.stageClearWait++;
					}
				}

				for (int i = 0; i < hostStageData.enemy.size(); i++) {
					if (hostStageData.enemy[i].status.life > 0) {
						hostStageData.enemy[i].gameObject.graphScale = 4;

						hostStageData.enemy[i].actionProgress++;

						Vector2D localPos;

						switch (hostStageData.enemy[i].type) {
						default:
							hostStageData.enemy[i].gameObject.graphNum = Animation(
								sandBagFakeNoseGraph, 0, 1, 30, hostStageData.enemy[i].animationProgress);
							break;

						case 1:
							hostStageData.enemy[i].gameObject.graphNum = Animation(
								sandBagFakeNoseGraph, 2, 3, 15, hostStageData.enemy[i].animationProgress);
							hostStageData.enemy[i].gameObject.rotation += 1;
							hostStageData.damageObject.push_back(newDamageObject(
								newGameObject(
									LocalPosition(0, 100, hostStageData.enemy[i].gameObject), 0, 2, 0, 1),
								hostStageData.enemy[0]));
							break;

						case 2:
							if (hostStageData.enemy[i].actionProgress >= 0 && hostStageData.enemy[i].actionProgress < 40) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 150) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, LocalPosition(
										rand() % 801 - 400, rand() % 801 - 400,
										hostStageData.enemy[i].gameObject),
									0.01f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRobotGraph, 0, 3, 8, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 200) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 204) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[4];
							}
							else if (hostStageData.enemy[i].actionProgress < 250) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[5];
							}
							else if (hostStageData.enemy[i].actionProgress < 500) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRobotGraph, 5, 7, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 21 - 70, rand() % 21 - 70,
											hostStageData.enemy[i].gameObject),
										0, 0.7, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 241 - 120, rand() % 1500,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 530) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[5];
							}
							else if (hostStageData.enemy[i].actionProgress < 536) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[4];
							}
							else if (hostStageData.enemy[i].actionProgress < 580) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 650) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, LocalPosition(
										rand() % 801 - 400, rand() % 801 - 400,
										hostStageData.enemy[i].gameObject),
									0.01f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRobotGraph, 0, 3, 8, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 675) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 680) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[8];
							}
							else if (hostStageData.enemy[i].actionProgress < 710) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[9];
							}
							else if (hostStageData.enemy[i].actionProgress < 712) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[10];
							}
							else if (hostStageData.enemy[i].actionProgress < 725) {
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
										5 + fmodf((float)rand(), 0.81f), 0, 1),
									hostStageData.enemy[0]));
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[11];
							}
							else if (hostStageData.enemy[i].actionProgress < 745) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRobotGraph[11];
							}
							else {
								hostStageData.enemy[i].actionProgress = 0;
							}
							break;

						case 3:
							if (hostStageData.enemy[i].actionProgress >= 0 && hostStageData.enemy[i].actionProgress < 40) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[0];
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 240) {
								if (hostStageData.enemy[i].actionProgress % 50 == 40) {
									localPos = LocalPosition(
										rand() % 801 - 400, rand() % 801 - 400,
										hostStageData.enemy[i].gameObject);
								}
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.02f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 0, 1, 6, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 300) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 2, 3, 3, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 303) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[4];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 295) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 241 - 120, rand() % 241,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 306) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[5];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 295) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 361 - 180, rand() % 361,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 309) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[6];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 295) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 481 - 240, rand() % 481,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 350) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[0];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 295) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								localPos = LocalPosition(
									rand() % 201 - 100, -(rand() % 101), hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 440) {
								if (hostStageData.enemy[i].actionProgress % 50 == 40) {
									localPos = LocalPosition(
										rand() % 201 - 100, -(rand() % 101),
										hostStageData.enemy[i].gameObject);
								}
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.02f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 0, 1, 6, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 500) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 2, 3, 3, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 503) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[4];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * -8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * 8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 241 - 120, rand() % 241,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 506) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[5];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * -8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * 8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 361 - 180, rand() % 361,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 509) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[6];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * -8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * 8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 481 - 240, rand() % 481,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 550) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseTankGraph[0];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * -8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											0, (hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											(hostStageData.enemy[i].actionProgress - 495) * 8,
											(hostStageData.enemy[i].actionProgress - 495) * 20,
											hostStageData.enemy[i].gameObject),
										0, 1.3f, 0, 1),
									hostStageData.enemy[0]));
								localPos = LocalPosition(rand() % 501 - 250, -500, hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 600) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.02f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 0, 1, 6, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 630) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(rand() % 11 - 5, -5, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 241 - 120, rand() % 481 - 240,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 660) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, 30, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 3 + fmodf((float)rand(), 0.91f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 700) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, -2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 481 - 240, rand() % 481 - 240,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								localPos = LocalPosition(rand() % 3001 - 1500, -500, hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 750) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.02f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 0, 1, 6, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 780) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(rand() % 11 - 5, -5, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 241 - 120, rand() % 481 - 240,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 810) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, 30, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 3 + fmodf((float)rand(), 0.91f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 850) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, -2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 481 - 240, rand() % 481 - 240,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								localPos = LocalPosition(rand() % 3001 - 1500, -700, hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 900) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.02f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 0, 1, 6, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 930) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(rand() % 11 - 5, -5, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 241 - 120, rand() % 481 - 240,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 960) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, 30, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 3 + fmodf((float)rand(), 0.91f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 1000) {
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, -2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseTankGraph, 7, 8, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 481 - 240, rand() % 481 - 240,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								localPos = LocalPosition(rand() % 3001 - 1500, -700, hostStageData.enemy[i].gameObject);
							}
							else {
								hostStageData.enemy[i].actionProgress = 0;
							}
							break;

						case 4:
							if (hostStageData.enemy[i].actionProgress >= 0 && hostStageData.enemy[i].actionProgress < 40) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseDrillGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 80) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 0, 1, 4, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 300) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 2, 3, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.enemy[i].gameObject.rotation += rand() % 61 - 30;
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(0, 10, hostStageData.enemy[i].gameObject);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(30, 100, hostStageData.enemy[i].gameObject), 0,
										0.8 + fmodf((float)rand(), 0.91f), 0, 1),
									hostStageData.enemy[0]));
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(-30, 100, hostStageData.enemy[i].gameObject), 0,
										0.8 + fmodf((float)rand(), 0.91f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 310) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 0, 1, 4, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 320) {
								hostStageData.enemy[i].gameObject.rotation = 0;
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 0, 1, 4, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 370) {
								hostStageData.enemy[i].gameObject.rotation = 0;
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 4, 5, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 501 - 250, rand() % 501 - 250,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 600) {
								if (hostStageData.enemy[i].actionProgress % 100 == 0) {
									localPos = LocalPosition(
										rand() % 801 - 400, rand() % 801 - 400,
										hostStageData.enemy[i].gameObject);
								}
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.02f);
								hostStageData.enemy[i].gameObject.rotation = 0;
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 6, 7, 2, hostStageData.enemy[i].animationProgress);
								for (int j = 0; j < 8; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 441 - 220, rand() % 441 - 220,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 2.5 + fmodf((float)rand(), 0.91f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 660) {
								hostStageData.enemy[i].gameObject.position = LocalPosition(
									rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseDrillGraph, 6, 7, 2, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 665) {
								hostStageData.enemy[i].gameObject.position = LocalPosition(
									rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseDrillGraph[8];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 8 + fmodf((float)rand(), 1.31f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 670) {
								hostStageData.enemy[i].gameObject.position = LocalPosition(
									rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseDrillGraph[9];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 12 + fmodf((float)rand(), 1.31f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 675) {
								hostStageData.enemy[i].gameObject.position = LocalPosition(
									rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseDrillGraph[10];
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 11 - 5, rand() % 11 - 5, hostStageData.enemy[i].gameObject),
										0, 24 + fmodf((float)rand(), 1.31f), 0, 1),
									hostStageData.enemy[0]));
							}
							else if (hostStageData.enemy[i].actionProgress < 685) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseDrillGraph[11];
							}
							else {
								hostStageData.enemy[i].actionProgress = 0;
							}
							break;

						case 5:
							if (hostStageData.enemy[i].actionProgress >= 0 && hostStageData.enemy[i].actionProgress < 40) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[0];
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 150) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.01f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRoidGraph, 0, 3, 8, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 200) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 204) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[4];
							}
							else if (hostStageData.enemy[i].actionProgress < 250) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[5];
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 350) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRoidGraph, 5, 7, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 21 - 70, rand() % 21 - 70,
											hostStageData.enemy[i].gameObject),
										0, 0.7, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 12; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 401 - 200, rand() % 1500,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 351) {
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 450) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRoidGraph, 5, 7, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 21 - 70, rand() % 21 - 70,
											hostStageData.enemy[i].gameObject),
										0, 0.7, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 24; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 601 - 300, rand() % 1500,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 451) {
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 500) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRoidGraph, 5, 7, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(
											rand() % 21 - 70, rand() % 21 - 70,
											hostStageData.enemy[i].gameObject),
										0, 0.7, 0, 1),
									hostStageData.enemy[0]));
								for (int j = 0; j < 48; j++) {
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(
												rand() % 801 - 400, rand() % 1500,
												hostStageData.enemy[i].gameObject),
											0, 0.3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 530) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[5];
							}
							else if (hostStageData.enemy[i].actionProgress < 536) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[4];
							}
							else if (hostStageData.enemy[i].actionProgress < 580) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[0];
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 650) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.01f);
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseRoidGraph, 0, 3, 8, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 675) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[0];
							}
							else if (hostStageData.enemy[i].actionProgress < 680) {
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[8];
							}
							else if (hostStageData.enemy[i].actionProgress < 710) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[9];
							}
							else if (hostStageData.enemy[i].actionProgress < 712) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[10];
							}
							else if (hostStageData.enemy[i].actionProgress < 775) {
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
										7 + fmodf((float)rand(), 0.81f), 0, 1),
									hostStageData.enemy[0]));
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[11];
							}
							else if (hostStageData.enemy[i].actionProgress < 780) {
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[8];
							}
							else if (hostStageData.enemy[i].actionProgress < 810) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[9];
							}
							else if (hostStageData.enemy[i].actionProgress < 812) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[10];
							}
							else if (hostStageData.enemy[i].actionProgress < 875) {
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
										8 + fmodf((float)rand(), 0.81f), 0, 1),
									hostStageData.enemy[0]));
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[11];
							}
							else if (hostStageData.enemy[i].actionProgress < 880) {
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[8];
							}
							else if (hostStageData.enemy[i].actionProgress < 910) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[9];
							}
							else if (hostStageData.enemy[i].actionProgress < 912) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[10];
							}
							else if (hostStageData.enemy[i].actionProgress < 975) {
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
										9 + fmodf((float)rand(), 0.81f), 0, 1),
									hostStageData.enemy[0]));
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[11];
							}
							else if (hostStageData.enemy[i].actionProgress < 980) {
								localPos = LocalPosition(
									rand() % 801 - 400, rand() % 801 - 400,
									hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[8];
							}
							else if (hostStageData.enemy[i].actionProgress < 1010) {
								hostStageData.enemy[i].gameObject.position = AveragePosition(
									hostStageData.enemy[i].gameObject.position, 1, localPos,
									0.03f);
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[9];
							}
							else if (hostStageData.enemy[i].actionProgress < 1012) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[10];
							}
							else if (hostStageData.enemy[i].actionProgress < 1075) {
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
										10 + fmodf((float)rand(), 0.81f), 0, 1),
									hostStageData.enemy[0]));
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseRoidGraph[11];
							}
							else {
								hostStageData.enemy[i].actionProgress = 0;
							}
							break;
						case 6:
							if (hostStageData.enemy[i].actionProgress >= 0 && hostStageData.enemy[i].actionProgress < 40) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseEmperorGraph, 0, 1, 3, hostStageData.enemy[i].animationProgress);
								localPos = LocalPosition(
									rand() % 1801 - 900, rand() % 1801 - 900,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 80) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[2];
							}
							else if (hostStageData.enemy[i].actionProgress < 560) {
								if (hostStageData.enemy[i].actionProgress % 80 < 30) {
									hostStageData.enemy[i].gameObject.position = AveragePosition(
										hostStageData.enemy[i].gameObject.position, 1, localPos,
										0.01f);
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[2];
								}
								else if (hostStageData.enemy[i].actionProgress % 80 < 50) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[2];
									hostStageData.enemy[i].gameObject.position = LocalPosition(
										rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
								}
								else {
									localPos = LocalPosition(
										rand() % 1801 - 900, rand() % 1801 - 900,
										hostStageData.enemy[i].gameObject);
									hostStageData.enemy[i].gameObject.graphNum = Animation(
										FakeNoseEmperorGraph, 2, 3, 2, hostStageData.enemy[i].animationProgress);
									hostStageData.enemy[i].gameObject.rotation += rand() % 7 + 3;
									hostStageData.enemy[i].gameObject.position = LocalPosition(
										rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
									for (int j = 0; j < 24; j++) {
										hostStageData.damageObject.push_back(newDamageObject(
											newGameObject(
												LocalPosition(
													rand() % 1500, rand() % 241 - 120,
													hostStageData.enemy[i].gameObject),
												0, 0.3f + fmodf((float)rand(), 0.91f), 0, 1),
											hostStageData.enemy[0]));
										hostStageData.damageObject.push_back(newDamageObject(
											newGameObject(
												LocalPosition(
													-(rand() % 1500), rand() % 241 - 120,
													hostStageData.enemy[i].gameObject),
												0, 0.3f + fmodf((float)rand(), 0.91f), 0, 1),
											hostStageData.enemy[0]));
									}
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 600) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseEmperorGraph, 0, 1, 3, hostStageData.enemy[i].animationProgress);
								localPos = LocalPosition(
									rand() % 1801 - 900, rand() % 1801 - 900,
									hostStageData.enemy[i].gameObject);
							}
							else if (hostStageData.enemy[i].actionProgress < 640) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[4];
							}
							else if (hostStageData.enemy[i].actionProgress < 1200) {
								if (hostStageData.enemy[i].actionProgress % 120 < 40) {
									hostStageData.enemy[i].gameObject.rotation += rand() % 32 + 3;
									hostStageData.enemy[i].gameObject.position = AveragePosition(
										hostStageData.enemy[i].gameObject.position, 1, localPos,
										0.02f);
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[4];
								}
								else if (hostStageData.enemy[i].actionProgress % 120 < 60) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[4];
									hostStageData.enemy[i].gameObject.position = LocalPosition(
										rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
								}
								else if (hostStageData.enemy[i].actionProgress % 120 < 100) {
									localPos = LocalPosition(
										rand() % 6001 - 3000, rand() % 6001 - 3000,
										hostStageData.enemy[i].gameObject);
									hostStageData.enemy[i].gameObject.graphNum = Animation(
										FakeNoseEmperorGraph, 4, 5, 2, hostStageData.enemy[i].animationProgress);
									hostStageData.enemy[i].gameObject.position = LocalPosition(
										rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
											3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
									for (int j = 0; j < 10; j++) {
										hostStageData.damageObject.push_back(newDamageObject(
											newGameObject(
												LocalPosition(
													0, (hostStageData.enemy[i].actionProgress % 2 + 2 * j) * 60,
													hostStageData.enemy[i].gameObject),
												0, 2, 0, 1),
											hostStageData.enemy[0]));
									}
									for (int j = 0; j < 24; j++) {
										hostStageData.damageObject.push_back(newDamageObject(
											newGameObject(
												LocalPosition(
													rand() % 601 - 300, rand() % 1500,
													hostStageData.enemy[i].gameObject),
												0, 0.3f + fmodf((float)rand(), 0.91f), 0, 1),
											hostStageData.enemy[0]));
									}
								}
								else {
									hostStageData.enemy[i].gameObject.rotation += 13;
									hostStageData.enemy[i].gameObject.graphNum = Animation(
										FakeNoseEmperorGraph, 4, 5, 2, hostStageData.enemy[i].animationProgress);
									hostStageData.enemy[i].gameObject.position = LocalPosition(
										rand() % 5 - 2, rand() % 5 - 2, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
											3 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
									for (int j = 0; j < 10; j++) {
										hostStageData.damageObject.push_back(newDamageObject(
											newGameObject(
												LocalPosition(
													0, (hostStageData.enemy[i].actionProgress % 2 + 2 * j) * 60,
													hostStageData.enemy[i].gameObject),
												0, 2, 0, 1),
											hostStageData.enemy[0]));
									}
									for (int j = 0; j < 24; j++) {
										hostStageData.damageObject.push_back(newDamageObject(
											newGameObject(
												LocalPosition(
													rand() % 601 - 300, rand() % 1500,
													hostStageData.enemy[i].gameObject),
												0, 0.3f + fmodf((float)rand(), 0.91f), 0, 1),
											hostStageData.enemy[0]));
									}
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 1240) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseEmperorGraph, 0, 1, 3, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 1280) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[6];
							}
							else if (hostStageData.enemy[i].actionProgress < 1660) {
								if (hostStageData.enemy[i].actionProgress % 80 < 6) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[7];
									hostStageData.enemy[i].gameObject.position =
										LocalPosition(40, 40, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(200, 200, hostStageData.enemy[i].gameObject), 0,
											4 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								else if (hostStageData.enemy[i].actionProgress % 80 < 20) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[8];
									hostStageData.enemy[i].gameObject.position =
										LocalPosition(3, 3, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(100, 100, hostStageData.enemy[i].gameObject), 0,
											2 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								else if (hostStageData.enemy[i].actionProgress % 80 < 26) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[9];
									hostStageData.enemy[i].gameObject.position =
										LocalPosition(-40, 40, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(-200, 200, hostStageData.enemy[i].gameObject), 0,
											4 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								else if (hostStageData.enemy[i].actionProgress % 80 < 40) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[10];
									hostStageData.enemy[i].gameObject.position =
										LocalPosition(-3, 3, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(-100, 100, hostStageData.enemy[i].gameObject), 0,
											2 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								else if (hostStageData.enemy[i].actionProgress % 80 < 46) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[7];
									hostStageData.enemy[i].gameObject.position =
										LocalPosition(40, 40, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(200, 200, hostStageData.enemy[i].gameObject), 0,
											4 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								else if (hostStageData.enemy[i].actionProgress % 80 < 60) {
									hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[8];
									hostStageData.enemy[i].gameObject.position =
										LocalPosition(10, 10, hostStageData.enemy[i].gameObject);
									hostStageData.damageObject.push_back(newDamageObject(
										newGameObject(
											LocalPosition(100, 100, hostStageData.enemy[i].gameObject), 0,
											2 + fmodf((float)rand(), 0.91f), 0, 1),
										hostStageData.enemy[0]));
								}
								else {
									hostStageData.enemy[i].gameObject.rotation += 5 + rand() % 9;
								}
							}
							else if (hostStageData.enemy[i].actionProgress < 1700) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseEmperorGraph, 0, 1, 3, hostStageData.enemy[i].animationProgress);
							}
							else if (hostStageData.enemy[i].actionProgress < 1740) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[11];
							}
							else if (hostStageData.enemy[i].actionProgress < 1860) {
								hostStageData.enemy[i].gameObject.graphNum = FakeNoseEmperorGraph[11];
								hostStageData.enemy[i].gameObject.position = LocalPosition(
									rand() % 9 - 4, rand() % 9 - 4, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(2, 0, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.rotation += rand() % 3;
							}
							else if (hostStageData.enemy[i].actionProgress < 2600) {
								hostStageData.enemy[i].gameObject.graphNum = Animation(
									FakeNoseEmperorGraph, 11, 12, 2, hostStageData.enemy[i].animationProgress);
								hostStageData.enemy[i].gameObject.position = LocalPosition(
									rand() % 9 - 4, rand() % 9 - 4, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.position =
									LocalPosition(2, 0, hostStageData.enemy[i].gameObject);
								hostStageData.enemy[i].gameObject.rotation += rand() % 2;
								hostStageData.damageObject.push_back(newDamageObject(
									newGameObject(
										LocalPosition(0, 0, hostStageData.enemy[i].gameObject), 0,
										(hostStageData.enemy[i].actionProgress - 1860) / 2 + fmodf((float)rand(), 0.91f),
										0, 1),
									hostStageData.enemy[0]));
							}
							else {
								hostStageData.enemy[i].actionProgress = 0;
							}
							break;
						}

						hostStageData.enemy[i].gameObject.arrive = true;

						int damageHitCount = 0;
						hostStageData.enemy[i].noseHit[0] = 0;
						hostStageData.enemy[i].noseHit[1] = 0;
						for (int j = 0; j < hostStageData.napnose.size(); j++) {
							if (hitCircle(hostStageData.enemy[i].gameObject, hostStageData.napnose[j].gameObject, 60) &&
								hostStageData.napnose[j].status.life > 0) {
								hostStageData.enemy[i].noseHit[0]++;
							}
						}
						for (int j = 0; j < hostStageData.MEGNOSE.size(); j++) {
							if (hitCircle(hostStageData.enemy[i].gameObject, hostStageData.MEGNOSE[j].gameObject, 60) &&
								hostStageData.MEGNOSE[j].status.life > 0) {
								hostStageData.enemy[i].noseHit[1]++;
							}
						}

						if (hostStageData.enemy[i].noseHit[0] != 0 && hostStageData.enemy[i].noseHit[1] != 0) {
							for (int j = 0; j < hostStageData.enemy[i].noseHit[0] + hostStageData.enemy[i].noseHit[1]; j++) {
								damageHitCount++;
								hostStageData.hitMark.push_back(hostStageData.enemy[i].gameObject);
								hostStageData.hitMark[hostStageData.hitMark.size() - 1].rotation += rand();
								hostStageData.hitMark[hostStageData.hitMark.size() - 1].position = LocalPosition(
									rand() % 121 - 60, rand() % 121 - 60, hostStageData.enemy[i].gameObject);
								hostStageData.hitMark[hostStageData.hitMark.size() - 1].scale /= damageHitCount / 3 + 1;
								Damage(hostStageData.enemy[i].status, seClip[1]);
							}
						}
						LifeBehavior(hostStageData.enemy[i].status, hostStageData.enemy[i].maxLife);
					}
					else {
						if (hostStageData.enemy[i].status.life < -50) {
							hostStageData.enemy[i].gameObject.arrive = false;
							PlaySoundMem(seClip[3], DX_PLAYTYPE_BACK, true);
							hostStageData.enemy[i].gameObject.graphNum = sandBagFakeNoseGraph[rand() % 4];
							SpreadShard(
								hostStageData.shards, 14, newVector2(0, 145), 0.21f, 1, 0.3f, 0.49f, 6,
								hostStageData.enemy[i].gameObject);
							SpreadShard(
								hostStageData.shards, 7, newVector2(0, 45), 0.11f, 0.85f, 0.5f, 0.6f, 20,
								hostStageData.enemy[i].gameObject);
							SpreadShard(
								hostStageData.explodes, 1, newVector2(0, 0), 1, 0, 1, 1, 80, hostStageData.enemy[i].gameObject);
						}
						else {
							hostStageData.enemy[i].status.life--;
							if (hostStageData.enemy[i].status.life % 8 == -3) {
								PlaySoundMem(seClip[2], DX_PLAYTYPE_BACK, true);
								hostStageData.enemy[i].gameObject.graphNum = sandBagFakeNoseGraph[rand() % 4];
								SpreadShard(
									hostStageData.shards, 19, newVector2(0, 225), 0.21f, 1, 0.1f, 0.49f, 4,
									hostStageData.enemy[i].gameObject);
								SpreadShard(
									hostStageData.shards, 5, newVector2(0, 45), 0.11f, 0.85f, 0.2f, 0.9f, 15,
									hostStageData.enemy[i].gameObject);
							}
							hostStageData.enemy[i].gameObject.rotation += 51;
							hostStageData.enemy[i].gameObject.position = LocalPosition(
								rand() % 101 - 50, rand() % 101 - 50, hostStageData.enemy[i].gameObject);
						}
					}

					pushBackField(hostStageData.enemy[i].gameObject);

					if (!hostStageData.enemy[i].gameObject.arrive) {
						hostStageData.enemy.erase(hostStageData.enemy.begin() + i);
					}
				}

				vector<WiperBeam> wipers = { currentWiper,otherWiper };

				NoseMove(
					hostStageData.napnose, noseGraph[0], maxNose, maxNoseLife, seClip[0], seClip[5], wipers,
					hostStageData.damageObject, hostStageData.damageMark, hostStageData.MEGNOSE, hostStageData.shards);
				NoseMove(
					hostStageData.MEGNOSE, noseGraph[1], maxNose, maxNoseLife, seClip[0], seClip[5], wipers,
					hostStageData.damageObject, hostStageData.damageMark, hostStageData.napnose, hostStageData.shards);

				for (int i = 0; i < hostStageData.napnose.size(); i++) {
					for (int ii = 0; ii < hostStageData.MEGNOSE.size(); ii++) {
						if (hitCircle(hostStageData.napnose[i].gameObject, hostStageData.MEGNOSE[ii].gameObject, 60) &&
							hostStageData.napnose[i].linePush <= 0 && hostStageData.MEGNOSE[ii].linePush <= 0) {
							CollideCircle(hostStageData.napnose[i].gameObject, hostStageData.MEGNOSE[ii].gameObject);
							CollideCircle(hostStageData.MEGNOSE[ii].gameObject, hostStageData.napnose[i].gameObject);
						}
					}
				}

				ShardsBehaviour(hostStageData.shards);
				ShardsBehaviour(hostStageData.explodes);

				if (hostStageData.napnose.size() <= 0 && hostStageData.MEGNOSE.size() <= 0 && cutSceneProgress > 1) {
					stageData.gameoverWait++;
					telopPos = 30;
				}

				if (stageData.gameoverWait > 200 && cutSceneProgress > 1) {
					gameoverTipsNum = (gameoverTipsNum + 1) % 5;
					nextScene = GAMEOVER;
				}
				if (stageData.stageClearWait > 270 && cutSceneProgress > 1) {
					SetNextScene(cutSceneProgress, nextScene, STAGESELECT);
				}
			}
			break;
		case GAMEOVER:
			bgmNum = 3;
			if (!currentWiper.userData->clicked && currentWiper.preClicked && telopPos <= 0) {
				SetNextScene(cutSceneProgress, nextScene, STAGESELECT);
			}
			break;
		}

		buttons = preButtons;
		preButtons = {};

		for (int i = 0; i < buttons.size(); i++) {
			buttons[i].gameObject.position.y += sin(clock() / pi / 300 - i / 2.0) * 5;
			ButtonBehaviour(buttons[i], currentWiper, seClip[4]);
		}

		if (stageData.stageClearWait > 110) {
			if (stageData.stageClearWait < 160) {
				telopPos = stageData.stageClearWait - 160;
			}
			else {
				telopPos = 0;
			}
		}
		if (scene == GAMEOVER) {
			if (telopPos > 0) {
				telopPos--;
			}
			else {
				telopPos = 0;
			}
		}

		if (cutSceneProgress < 1) {
			cutSceneProgress += 0.04f;
		}
		else {
			cutInNose = !cutInNose;
		}
		if (cutSceneProgress >= 0.5f) {
			scene = nextScene;
		}

		WiperBeamUpdate(otherWiper);
		*otherWiper.userData = otherPlayer;

		WiperBeamUpdate(currentWiper);
		currentPlayer.mouse = GetMousePositionToV2D();
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0 && !currentPlayer.clicked) {
			currentPlayer.pin = GetMousePositionToV2D();
		}
		currentPlayer.clicked = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;


		if (!CheckSoundMem(audioClip[bgmNum]) && bgmSwitch) {
			PlaySoundMem(audioClip[bgmNum], DX_PLAYTYPE_BACK, true);
		}
		for (int i = 0; i < sizeof(audioClip) / sizeof(*audioClip); i++) {
			if (i != bgmNum || !bgmSwitch || stageData.stageClearWait > 10 || stageData.gameoverWait > 1) {
				StopSoundMem(audioClip[i]);
			}
		}

		for (int i = 0; i < sizeof(seClip) / sizeof(*seClip); i++)
		{
			if (!seSwitch)
			{
				StopSoundMem(seClip[i]);
			}
		}

		// 描画処理

		DrawRotaGraph(WIN_WIDTH / 2, WIN_HEIGHT / 2, 4.7, 0, backGround, TRUE);

		RenderObject(tutorialWindow);
		RenderObject(tutorialFlag);

		if (stageData.stageClearWait > 110) {
			DrawCircle(
				WIN_WIDTH / 2, WIN_HEIGHT / 2, (stageData.stageClearWait - 110) * 60, GetColor(255, 255, 135),
				TRUE);
		}
		if (stageData.stageClearWait > 125) {
			DrawCircle(
				WIN_WIDTH / 2, WIN_HEIGHT / 2, (stageData.stageClearWait - 125) * 60, GetColor(255, 60, 180),
				TRUE);
		}
		if (stageData.stageClearWait > 140) {
			DrawCircle(
				WIN_WIDTH / 2, WIN_HEIGHT / 2, (stageData.stageClearWait - 140) * 60, GetColor(60, 255, 180),
				TRUE);
		}

		for (int i = 0; i < hostStageData.damageObject.size(); i++) {
			int r = 0, gb = 0;
			if (AttackAble(hostStageData.damageObject[i])) {
				r = 255;
				gb = 0;
			}
			else {
				r = 100;
				gb = 100;
			}
			DrawCircle(
				(int)hostStageData.damageObject[i].gameObject.position.x,
				(int)hostStageData.damageObject[i].gameObject.position.y,
				(int)(hostStageData.damageObject[i].gameObject.scale * 60), GetColor(255, 255, 255), TRUE, 3);
			DrawCircle(
				(int)hostStageData.damageObject[i].gameObject.position.x,
				(int)hostStageData.damageObject[i].gameObject.position.y,
				(int)(hostStageData.damageObject[i].gameObject.scale * 60), GetColor(r, gb, gb), TRUE);
		}

		for (int i = 0; i < hostStageData.enemy.size(); i++) {
			RenderObject(hostStageData.enemy[i].gameObject);
			DrawGage(50, 70, hostStageData.enemy[i].status.life * 1.0f / hostStageData.enemy[i].maxLife, hostStageData.enemy[i].gameObject);
		}

		for (int i = 0; i < hostStageData.napnose.size(); i++) {
			RenderObject(hostStageData.napnose[i].gameObject);
			DrawGage(50, 70, hostStageData.napnose[i].status.life * 1.0f / maxNoseLife, hostStageData.napnose[i].gameObject);
		}
		for (int i = 0; i < hostStageData.MEGNOSE.size(); i++) {
			RenderObject(hostStageData.MEGNOSE[i].gameObject);
			DrawGage(
				50, 70, hostStageData.MEGNOSE[i].status.life * 1.0f / maxNoseLife * 1.0f, hostStageData.MEGNOSE[i].gameObject);
		}

		for (int i = 0; i < hostStageData.hitMark.size(); i++) {
			DrawTriangle(
				(int)LocalPosition(0, 50, hostStageData.hitMark[i]).x, (int)LocalPosition(0, 50, hostStageData.hitMark[i]).y,
				(int)LocalPosition(22, -10, hostStageData.hitMark[i]).x,
				(int)LocalPosition(22, -10, hostStageData.hitMark[i]).y,
				(int)LocalPosition(-26, -16, hostStageData.hitMark[i]).x,
				(int)LocalPosition(-26, -16, hostStageData.hitMark[i]).y,
				GetColor(255, 100, 100), TRUE);
			DrawTriangle(
				(int)LocalPosition(0, -50, hostStageData.hitMark[i]).x, (int)LocalPosition(0, -50, hostStageData.hitMark[i]).y,
				(int)LocalPosition(26, 16, hostStageData.hitMark[i]).x, (int)LocalPosition(26, 16, hostStageData.hitMark[i]).y,
				(int)LocalPosition(-22, 10, hostStageData.hitMark[i]).x,
				(int)LocalPosition(-22, 10, hostStageData.hitMark[i]).y,
				GetColor(255, 100, 100), TRUE);
		}

		for (int i = 0; i < hostStageData.damageMark.size(); i++) {
			for (int j = 0; j < 3; j++) {
				DrawTriangle(
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).x,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).y,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).x,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).y,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).x,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).y,
					GetColor(150, 0, 0), TRUE);
				DrawLine(
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).x,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).y,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).x,
					(int)LocalPosition(rand() % 201 - 100, rand() % 201 - 100, hostStageData.damageMark[i]).y,
					GetColor(255, 150, 150));
			}
		}

		for (int i = 0; i < hostStageData.explodes.size(); i++) {
			DrawCircle(
				(int)hostStageData.explodes[i].gameObject.position.x, (int)hostStageData.explodes[i].gameObject.position.y,
				(int)(hostStageData.explodes[i].gameObject.scale * 3000 * sin(hostStageData.explodes[i].lifetime / 80.0 * pi)),
				GetColor(255, 255, 255), TRUE);
		}
		for (int i = 0; i < hostStageData.shards.size(); i++) {
			RenderObject(hostStageData.shards[i].gameObject);
		}

		if (stageData.stageClearWait > 110) {
			DrawRotaGraph(
				WIN_WIDTH / 3 * 2,
				WIN_HEIGHT / 3 + (int)((telopPos * telopPos * telopPos * telopPos) * 0.001f), 4, 0,
				clearGraph, TRUE);
		}

		if (stageData.gameoverWait > 100) {
			DrawCircle(
				WIN_WIDTH / 2, WIN_HEIGHT / 2, (stageData.gameoverWait - 100) * 90, GetColor(0, 0, 0),
				TRUE);
		}

		SetBackgroundColor(80, 120, 250);

		switch (scene) {
		case TITLESCREEN:
			DrawRotaGraph(
				WIN_WIDTH / 2, (int)(WIN_HEIGHT / 3 + sin(clock() / pi / 300) * 8), 2, 0, title, TRUE);
			DrawRotaGraph(50, 50, 2, clock() / pi / 200, noseGraph[0], TRUE);
			DrawRotaGraph(
				WIN_WIDTH - 50, WIN_HEIGHT - 50, 2, clock() / pi / 200, noseGraph[1], TRUE);
			DrawString(
				WIN_WIDTH / 2 - 50, WIN_HEIGHT - 30, "2021-2023 あるは                Ver.1.0.2",
				GetColor(255, 255, 255));
			if (time(nullptr) % 2) {
				DrawString(
					WIN_WIDTH / 2 - 50, WIN_HEIGHT - 140, "CLICK   START", GetColor(255, 255, 255));
			}
			break;
		case SETTING:
			DrawRotaGraph(50, 50, 2, clock() / pi / 200, noseGraph[0], TRUE);
			DrawRotaGraph(
				WIN_WIDTH - 50, WIN_HEIGHT - 50, 2, clock() / pi / 200, noseGraph[1], TRUE);
			DrawString(40, 50, "オプション", GetColor(255, 255, 255));
			break;
		case MAINMENU:
			DrawRotaGraph(50, 50, 2, clock() / pi / 200, noseGraph[0], TRUE);
			DrawRotaGraph(
				WIN_WIDTH - 50, WIN_HEIGHT - 50, 2, clock() / pi / 200, noseGraph[1], TRUE);
			DrawString(40, 50, "メインメニュー", GetColor(255, 255, 255));
			break;
		case STAGESELECT:
			DrawRotaGraph(50, 50, 2, clock() / pi / 200, noseGraph[0], TRUE);
			DrawRotaGraph(
				WIN_WIDTH - 50, WIN_HEIGHT - 50, 2, clock() / pi / 200, noseGraph[1], TRUE);
			DrawString(40, 50, "メインゲーム", GetColor(255, 255, 255));
			break;
		case CREDITS:
			DrawString(40, 50, "クレジット", GetColor(255, 255, 255));
			DrawString(
				WIN_WIDTH / 4, 100,
				"システム\n あるは\n\nグラフィック\n あるは\n\n出演\n なぷり\n "
				"メグリ\n\nサウンド\n BGM\n  あるは\n SE\n  あるは\n\nプログラム\n "
				"あるは\n\nデバッグ\n あるは\n\nディレクター\n あるは\n\n",
				GetColor(255, 255, 255));
			break;
		case GAMEOVER:
			SetBackgroundColor(0, 0, 0);
			DrawRotaGraph(
				WIN_WIDTH / 2,
				(int)(WIN_HEIGHT / 4 - (telopPos * telopPos * telopPos * telopPos) * 0.0005f +
					sin(clock() / pi / 300) * 8),
				4, 0, gameoverGraph, TRUE);
			DrawString(
				40 - (telopPos * telopPos * telopPos * telopPos) * 0.0005, WIN_HEIGHT / 7 * 4,
				gameoverTipsText[gameoverTipsNum].c_str(), GetColor(255, 255, 255));
			if (time(nullptr) % 2 && telopPos <= 0) {
				DrawString(
					WIN_WIDTH / 2 - 200, WIN_HEIGHT - 100, "クリックして　ステージセレクトへ",
					GetColor(255, 255, 255));
			}
			break;
		}

		for (int i = 0; i < buttons.size(); i++) {
			DrawButton(buttons[i]);
		}

		//DrawMegaGage(hostStageData.napnose, hostStageData.MEGNOSE, maxNose, maxNoseLife, hostStageData.enemy);

		if (cutSceneProgress >= 0 && cutSceneProgress < 1) {
			DrawRotaGraph(
				WIN_WIDTH / 2, WIN_HEIGHT / 2, sin(cutSceneProgress * pi) * 12, 0,
				noseGraph[cutInNose], TRUE);
		}

		if (!bgmSwitch) {
			DrawString(WIN_WIDTH - 120, 0, "BGM : OFF", GetColor(180, 0, 0));
		}
		if (!seSwitch) {
			DrawString(WIN_WIDTH - 120, 26, "SE : OFF", GetColor(180, 0, 0));
		}
		if (!bgmSwitch || !seSwitch) {
			DrawString(
				WIN_WIDTH - 370, 52, "メインメニュー > オプションで\nオンにすることができます",
				GetColor(180, 0, 0));
		}

		DrawWiperBeam(otherWiper);
		DrawWiperBeam(currentWiper);

		//---------  ここまでにプログラムを記述  ---------//
		// (ダブルバッファ)裏面
		ScreenFlip();

		// 20ミリ秒待機(疑似60FPS)
		WaitTimer(20);

		// Windowsシステムからくる情報を処理する
		if (ProcessMessage() == -1)
		{
			break;
		}

		// ESCキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)
		{
			break;
		}
	}
	InitGraph();

	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}
// eof