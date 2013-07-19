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
#include <sstream>
#include "gameObject.h"

using namespace std;

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define PI atan(1.0f)*4
#define BEATDIST 41.75

#define QUARTER 1
#define EIGHTH 0.5
#define SIXTEENTH 0.25
#define THIRTYSECOND 0.125

class Game {

private:
	int screen, menuSelection, song;
	float old, cur;
	int values[100];
	int index;
	float diff;

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
	RECT start1, start2, speed, quit, bottom, explosion;

	// levels
	LPDIRECT3DTEXTURE9 targetTexture, explosionTexture, levelBackgroundTexture;
	D3DXVECTOR3 bgPos, playerTarget;
	D3DXMATRIX spriteManip, rotation, scaling, translation1, translation2;
	RECT bgTop, bgBottom, target, arrow, topDisplay1, topDisplay2, ratingDisplay;
	vector<GameObject> arrows;
	vector<GameObject> stepZone;
	bool focus, invincible, spellcard1, spellcard2, clear, ready;
	int leveltime, curAlpha, curFrame, curRow, moveRate, fireDirection, hits, score, level, lastCol, ratingTime, waitTime;
	int cols[100];
	float currentT, offset, speedMod, currentRating, bpm;

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
	void setRects();
	void loadTextures();
	void drawArrows();
	void drawStepZone();
	void scrollBackground();
	void sceneryParticles();
	void drawTitle();
	void drawTextAndNumber(LPCWSTR text, float num, RECT pos, D3DCOLOR fontColor);
	void moveObjects();
	void resetMatrices();
	void rotateBullets(float angle, size_t i);
	void placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type, float speed);
	void placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type, float speed);
	void setArrows(float start, float end, float type);
	void judge(float distance);
	D3DXMATRIX scale(D3DXMATRIX translation1, D3DXMATRIX translation2, float x, float y, D3DXMATRIX scaling, float xFactor, float yFactor);
	D3DXVECTOR3 rotateVector(D3DXVECTOR3 vec, float angle, size_t direction);
};

#endif