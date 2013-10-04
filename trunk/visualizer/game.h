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
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;

	IBaseFilter *pGrabberF;
    ISampleGrabber *pGrabber;
	IBaseFilter *pNullF;
	vector<BYTE> sampleBuffer;
	BYTE* initpBuffer;

	LPD3DXFONT font;
	D3DCOLOR fontColor;
	RECT start1, start2, speed, quit, bottom, explosion, topDisplay1, topDisplay2, ratingDisplay;
	LPDIRECT3DTEXTURE9 targetTexture;
	vector<GameObject> notes, stepZone;
	MusicReader musicReader;

	int screen, song;
	int menuSelection, ratingTime;
	int waitTime[100];
	float cols[100];
	float offset, speedMod, currentRating, bpm;
	int index;
	float test;
	float time;
	bool grabbed;

public:
	void setHwnd(HWND _hwnd);
	void initGame();
	void gameloop();
	void render();
	void handleInput();
	void updatePositions();
	void cleanup();

	HRESULT Game::IsPinConnected(IPin *pPin, BOOL *pResult);
	HRESULT Game::IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult);
	HRESULT Game::MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult);
	HRESULT Game::FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT Game::ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
	HRESULT Game::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);

	void initMenuScreen();
	void initGameScreen();
	void drawTextAndNumber(LPCWSTR text, float num, RECT pos, D3DCOLOR fontColor);
	void toggleMenuText(int selection);
	void setRects();
	void loadTextures();
	void loadNoteChart(int song_);
	void drawNotes();
	void drawStepZone();
	void moveObjects();
	void playMusic(int song_);
	void Game::getMusicSamples();
	void placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type);
	void placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type);
	void setNotes(float start, float end, float type);
	void hitNote(int column);
	void judge(float distance);
	void changeSpeed(float difference, int direction);
	void DrawLine(LPDIRECT3DDEVICE9 Device_Interface,int bx,int by,int bw,D3DCOLOR COLOR);
	float* convert16BitToFloat(vector<BYTE> samples);
};

#endif