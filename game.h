/*game.h
The core class of the game.  Runs the game loop and controls the use of all other classes related to the game.  
Handles drawing of game objects and game control. */

#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <DShow.h>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include "bullet.h"
#include "enemy.h"
#include "level.h"
#include "particleSystem.h"

using namespace std;

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define PI atan(1.0f)*4
#define BPM 170
#define COL1 20
#define COL2 120
#define COL3 220
#define COL4 320

class Game {

private:
	int screen, menuSelection;
	float old, cur;
	int index;
	float diff[10];

	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 pDev;
	HWND hwnd;
	LPD3DXSPRITE gameSprites;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;

	// menu
	LPDIRECT3DTEXTURE9 menuBackgroundTexture;
	LPD3DXFONT font;
	D3DCOLOR fontColor, fontColor2;
	RECT start, quit;

	// levels
	LPDIRECT3DTEXTURE9 targetTexture, explosionTexture, levelBackgroundTexture;
	D3DXVECTOR3 bgPos, playerTarget;
	D3DXMATRIX spriteManip, rotation, scaling, translation1, translation2;
	RECT bgTop, bgBottom, levelText, subText1, drawExplosion, topRight, target, arrow;
	vector<GameObject> arrows;
	vector<GameObject> stepZone;
	map<string,RECT> drawBoundaries;
	bool focus, invincible, spellcard1, spellcard2, clear;
	int leveltime, curAlpha, curFrame, curRow, moveRate, fireDirection, hits, score, level;
	float currentT, offset, speedMod;

public:
	void setHwnd(HWND _hwnd);
	void initd3d();
	void gameloop();
	void render();
	void handleInput();
	void updatePositions();
	void cleanup();

	void initMenuScreen();
	void initGameScreen();
	void loadTextures();
	void drawArrows();
	void drawStepZone();
	void scrollBackground();
	void sceneryParticles();
	void drawTitle();
	void drawTextAndNumber(LPCWSTR text, int num, RECT pos, D3DCOLOR fontColor);
	void moveObjects();
	void resetMatrices();
	void rotateBullets(float angle, size_t i);
	void placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, int type, float speed);
	D3DXMATRIX scale(D3DXMATRIX translation1, D3DXMATRIX translation2, float x, float y, D3DXMATRIX scaling, float xFactor, float yFactor);
	D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, float angle, size_t direction);
};

#endif