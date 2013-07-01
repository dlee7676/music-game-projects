#include "game.h"

/* setHwnd(): used to bring the window handle from the main function into the Game object for initializing Direct3D */
void Game::setHwnd(HWND _hwnd) {
	hwnd = _hwnd;
}

/* initd3d(): initializes a Direct3D object */
void Game::initd3d() {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	// sets the presentation parameters
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDev);
	
	index = 0;

	screen = 0;
	initMenuScreen();
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error initializing DirectShow"), TEXT("Error"), MB_ICONERROR);
	}
}

/* gameloop(): function run repeatedly in the message loop, executes drawing code and responds to game controls */
void Game::gameloop() {
	render();
	handleInput();
	updatePositions();
}

/* handleInput(): contains responses to key presses of the game controls */
void Game::handleInput() {
	// Esc returns to the menu screen from gameplay
	if (GetAsyncKeyState(VK_ESCAPE) && screen != 0) {
		screen = 0;
		initMenuScreen();
		HRESULT hr = pControl->Stop();
	}

	// controls for the menu screen
	switch (screen) {
		case 0: {
			// move between options
			if (GetAsyncKeyState(VK_DOWN)) {
				menuSelection++;
				Sleep(150);
			}
			if (GetAsyncKeyState(VK_UP)) {
				menuSelection--;
				Sleep(150);
			}
			// select an option
			if (GetAsyncKeyState(VK_RETURN)) {
				if (menuSelection == 0) {
					screen = 1;
					initGameScreen();
					//menuBackgroundTexture->Release();
				}
				if (menuSelection == 1)
					PostQuitMessage(0);
			}
			// loop the menu selections
			if (menuSelection < 0)
				menuSelection = 1;
			if (menuSelection > 1)
				menuSelection = 0;
			break;
		}

		case 1: {
			if (GetAsyncKeyState('D')) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].getPos(0) == COL1 && abs(stepZone[0].getPos(1) - arrows[i].getPos(1)) < 10)
						arrows[i].setActive(false);
				}
			}
			if (GetAsyncKeyState('F')) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].getPos(0) == COL2 && abs(stepZone[1].getPos(1) - arrows[i].getPos(1)) < 10)
						arrows[i].setActive(false);
				}
			}
			if (GetAsyncKeyState('J')) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].getPos(0) == COL3 && abs(stepZone[2].getPos(1) - arrows[i].getPos(1)) < 10)
						arrows[i].setActive(false);
				}
			}
			if (GetAsyncKeyState('K')) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].getPos(0) == COL4 && abs(stepZone[3].getPos(1) - arrows[i].getPos(1)) < 10)
						arrows[i].setActive(false);
				}
			}
			if (GetAsyncKeyState(VK_RETURN)) {
				cur = arrows[0].getPos(1);
				diff[index] = cur - old;
				old = arrows[0].getPos(1);
				index++;
				if (index >= 10)
					index = 0;
			}
			if (GetAsyncKeyState('C')) {
				int a = 0;
				int b = 1;
			}
			break;
		}
	}
}

/* render(): determines the positions of game objects and draws them on the screen. */
void Game::render() {



	pDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pDev->BeginScene();
	switch(screen) {
		case 0: {
			// draw menu screen
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			gameSprites->Draw(menuBackgroundTexture, NULL, NULL, &D3DXVECTOR3(0,0,0), 0xFFFFFFFF);
			gameSprites->End();
			if (menuSelection == 0)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			font->DrawText(NULL, TEXT("Start Game"), -1, &start, 0, fontColor);
			if (menuSelection == 1)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			font->DrawText(NULL, TEXT("Quit"), -1, &quit, 0, fontColor);
			break;
		}
		case 1: {
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			// draw the game elements
			//gameSprites->Draw(menuBackgroundTexture, &target, NULL, &D3DXVECTOR3(20,20,0), 0xFFFFFFFF);
			drawStepZone();
			drawArrows();
			gameSprites->End();
			break;
		}
	}
	pDev->EndScene();
	pDev->Present(NULL, NULL, NULL, NULL);
}

void Game::updatePositions() {
	moveObjects();
}

/* cleanup(): frees the memory associated with the game's objects. */
void Game::cleanup() {
	d3d->Release();
	pDev->Release();
	gameSprites->Release();
	font->Release();
}

void Game::initMenuScreen() {
	// screen 0
	if (FAILED(D3DXCreateSprite(pDev, &gameSprites))) {
		MessageBox(hwnd, TEXT("Error Loading Sprite"), TEXT("Error"), MB_ICONERROR);
		return;
	} 
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("prismriver.jpg"), &menuBackgroundTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	D3DXCreateFont(pDev, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Edwardian Script ITC"), &font); 
	start.left=SCREEN_WIDTH*3/5;
	start.right=SCREEN_WIDTH*9/10;
	start.top=SCREEN_HEIGHT*3/5;
	start.bottom=start.top+45;
	quit.left=start.left;
	quit.right=start.right;
	quit.top=start.bottom+30;
	quit.bottom=quit.top+45;
	menuSelection = 0;
	score = 0;
}

void Game::initGameScreen() {
	loadTextures();	
	D3DXCreateFont(pDev, 40, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic Demi"), &font); 
	srand((unsigned int)(time(0)));
	fontColor2 = D3DCOLOR_ARGB(255,240,240,240);
	curFrame = 0;
	curRow = 0;
	curAlpha = 50;
	currentT = 0;
	hits = 0;
	arrow.left=0;
	arrow.top=1050;
	arrow.right=55;
	arrow.bottom=1115;
	target.left = 333;
	target.top = 1988;
	target.right = 380;
	target.bottom = 2056;

	placeObject(&stepZone, COL1, 20, 0, target, 0, 1);
	placeObject(&stepZone, COL2, 20, 0, target, 0, 1);
	placeObject(&stepZone, COL3, 20, 0, target, 0, 1);
	placeObject(&stepZone, COL4, 20, 0, target, 0, 1);
	float time = 0.1;
	speedMod = 1;
	placeObject(&arrows, 500, 700, 0, arrow, 1, 1);
	placeObject(&arrows, COL1, 800*time*speedMod, 0, arrow, 1, 1);
	old = arrows[0].getPos(1);
	time = 1.2;
	placeObject(&arrows, COL2, 800*time*speedMod, 0, arrow, 1, 1);
	time = 1.4;
	placeObject(&arrows, COL3, 800*time*speedMod, 0, arrow, 1, 1);
	time = 1.6;
	placeObject(&arrows, COL4, 800*time*speedMod, 0, arrow, 1, 1);

	IGraphBuilder *pGraph;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	hr = pGraph->RenderFile(TEXT("rem3.mp3"), NULL);
	//hr = pControl->Run();
}

/* void loadTextures(size_t level_): loads the textures needed for a level. */

void Game::loadTextures() {
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("enemySprites.png"), &targetTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("explosionSpriteSheet.png"), &explosionTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("forest.png"), &levelBackgroundTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
}

void Game::drawStepZone() {
	for (size_t i = 0; i < stepZone.size(); i++) {
		if (stepZone[i].getType()==0)
			gameSprites->Draw(targetTexture, &target, NULL, &stepZone[i].getPos(), 0xFFFFFFFF);
	}
}

void Game::drawArrows() {
	// draw enemies
	for (size_t i = 0; i < arrows.size(); i++) {
		if (arrows[i].isActive())
			gameSprites->Draw(targetTexture, &arrow, NULL, &arrows[i].getPos(), 0xFFFFFFFF);
	}
}

void Game::moveObjects() {
	for (size_t i = 0; i < arrows.size(); i++) {
		arrows[i].move(0,-2*speedMod,0);
	}
}

void Game::placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, int type, float speed) {
	GameObject next;
	next.init(x,y,z,bounds,type,speed);
	vec->push_back(next);
}

/* void resetMatrices(): returns all transformation matrices to identity matrices. */
void Game::resetMatrices() {
	D3DXMatrixIdentity(&spriteManip);
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixIdentity(&scaling);
	D3DXMatrixIdentity(&translation1);
	D3DXMatrixIdentity(&translation2);
}