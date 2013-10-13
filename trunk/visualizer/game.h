/* class Game
The core class of the game.  Runs the game loop and controls the use of all other classes related to the game.  
Handles drawing of game objects and game control. */

#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <DShow.h>
#include <assert.h>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include <sstream>
#include <fftw3.h>
#include "gameObject.h"
#include "musicReader.h"
#include "qedit.h"

using namespace std;

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

class Game {

private:
	LPDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 pDev;
	HWND hwnd;
	LPD3DXSPRITE gameSprites;
	LPD3DXFONT font;
	D3DCOLOR fontColor;
	RECT start1, start2, speed, quit, bottom, explosion, topDisplay1, topDisplay2, ratingDisplay;
	LPDIRECT3DTEXTURE9 targetTexture;
	vector<BYTE> sampleBuffer;
	MusicReader musicReader;
	fftw_complex *in, *out;
	
	
	int screen, song;
	int menuSelection;
	float speedMod, bpm;
	int index;
	float time;
	bool grabbed, run;

public:
	void setHwnd(HWND _hwnd);
	void initGame();
	void gameloop();
	void render();
	void handleInput();
	void updatePositions();
	void cleanup();
	void initMenuScreen();
	void initGameScreen();
	void drawTextAndNumber(LPCWSTR text, float num, RECT pos, D3DCOLOR fontColor);
	void toggleMenuText(int selection);
	void setRects();
	void loadTextures();
	void drawSamples();
	unsigned long Game::upper_power_of_two(unsigned long v);

	// not currently used
	void loadNoteChart(int song_);
	void drawStepZone();
	void moveObjects();
	void playMusic(int song_);
	void getMusicSamples();
	void placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type);
	void placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type);
	void setNotes(float start, float end, float type);
	void hitNote(int column);
	void judge(float distance);
	void changeSpeed(float difference, int direction);
};

#endif