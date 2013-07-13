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

	initMenuScreen();
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error initializing DirectShow"), TEXT("Error"), MB_ICONERROR);
	}
}

/* gameloop(): function run repeatedly in the message loop, executes drawing code and responds to game controls */
void Game::gameloop() {
	updatePositions();
	render();
	handleInput();
}

/* handleInput(): contains responses to key presses of the game controls */
void Game::handleInput() {
	// Esc returns to the menu screen from gameplay
	if (GetAsyncKeyState(VK_ESCAPE) && screen != 0) {
		screen = 0;
		initMenuScreen();
		stepZone.clear();
		arrows.clear();
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
			if (GetAsyncKeyState(VK_LEFT)) {
				if (menuSelection == 2 && speedMod > 0.25) {
					speedMod-=0.25;
				}
				Sleep(150);
			}
			if (GetAsyncKeyState(VK_RIGHT)) {
				if (menuSelection == 2) {
					speedMod+=0.25;
				}
				Sleep(150);
			}
			// select an option
			if (GetAsyncKeyState(VK_RETURN)) {
				if (menuSelection == 0) {
					screen = 1;
					song = 0;
					bpm = 170;
					initGameScreen();
					//menuBackgroundTexture->Release();
				}
				if (menuSelection == 1) {
					screen = 1;
					song = 1;
					bpm = 75;
					initGameScreen();
					//menuBackgroundTexture->Release();
				}
				if (menuSelection == 3)
					PostQuitMessage(0);
			}
			// loop the menu selections
			if (menuSelection < 0)
				menuSelection = 3;
			if (menuSelection > 3)
				menuSelection = 0;
			break;
		}

		case 1: {
			if (GetAsyncKeyState('D') && waitTime <= 0) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (!arrows[i].isExploding() && arrows[i].getPos(0) == cols[0] && abs(stepZone[0].getPos(1) + 5 - arrows[i].getPos(1)) <= 45) {
						arrows[i].setExploding(true);
						currentRating = abs(stepZone[0].getPos(1) + 5 - arrows[i].getPos(1));
						ratingTime = 30;
						//waitTime=20;
						break;
					}
				}
			}
			if (GetAsyncKeyState('F') && waitTime <= 0) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (!arrows[i].isExploding() && arrows[i].getPos(0) == cols[1] && abs(stepZone[1].getPos(1) + 5 - arrows[i].getPos(1)) <= 30) {
						arrows[i].setExploding(true);
						currentRating = abs(stepZone[0].getPos(1) + 5 - arrows[i].getPos(1));
						ratingTime = 30;
						//waitTime=20;
						break;
					}
				}
			}
			if (GetAsyncKeyState('J') && waitTime <= 0) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (!arrows[i].isExploding() && arrows[i].getPos(0) == cols[2] && abs(stepZone[2].getPos(1) + 5 - arrows[i].getPos(1)) <= 30) {
						arrows[i].setExploding(true);
						currentRating = abs(stepZone[0].getPos(1) + 5 - arrows[i].getPos(1));
						ratingTime = 30;
						//waitTime=20;
						break;
					}
				}
			}
			if (GetAsyncKeyState('K') && waitTime <= 0) {
				for (size_t i = 0; i < arrows.size(); i++) {
					if (!arrows[i].isExploding() && arrows[i].getPos(0) == cols[3] && abs(stepZone[3].getPos(1) + 5 - arrows[i].getPos(1)) <= 30) {
						arrows[i].setExploding(true);
						currentRating = abs(stepZone[0].getPos(1) + 5 - arrows[i].getPos(1));
						ratingTime = 30;
						//waitTime=20;
						break;
					}
				}
			}
			if (GetAsyncKeyState('G') && waitTime <= 0) {
				float lastMod = speedMod;
				if (speedMod > 1)
					speedMod--;
				for(size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].isActive())
						arrows[i].setPos(arrows[i].getPos(0), arrows[i].getPos(1)*(speedMod/lastMod), 0);
				}
				waitTime=20;
			}
			if (GetAsyncKeyState('H') && waitTime <= 0) {
				float lastMod = speedMod;
				speedMod++;
				for(size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].isActive())
						arrows[i].setPos(arrows[i].getPos(0), arrows[i].getPos(1)*(speedMod/lastMod), 0);
				}
				waitTime=20;
			}
			else waitTime--;
			if (GetAsyncKeyState('1')) {
				float lastMod = speedMod;
				if (speedMod > 0.25)
					speedMod-=0.25;
				for(size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].isActive())
						arrows[i].setPos(arrows[i].getPos(0), arrows[i].getPos(1)*(speedMod/lastMod), 0);
				}
				Sleep(100);
			}
			if (GetAsyncKeyState('2')) {
				float lastMod = speedMod;
				speedMod+=0.25;
				for(size_t i = 0; i < arrows.size(); i++) {
					if (arrows[i].isActive())
						arrows[i].setPos(arrows[i].getPos(0), arrows[i].getPos(1)*(speedMod/lastMod), 0);
				}
				Sleep(100);
			}
			if (GetAsyncKeyState(VK_RETURN)) {
				values[index] = arrows[1].getPos(1);
				diff = abs(values[index] - values[index-1]);
				index++;
				if (index >= 100)
					index = 1;
				Sleep(150);
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
			//gameSprites->Draw(menuBackgroundTexture, NULL, NULL, &D3DXVECTOR3(0,0,0), 0xFFFFFFFF);
			gameSprites->End();
			if (menuSelection == 0)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			font->DrawText(NULL, TEXT("Play REM III (BPM 170)"), -1, &start1, 0, fontColor);
			if (menuSelection == 1)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			font->DrawText(NULL, TEXT("Play Destination Unknown (BPM 75)"), -1, &start2, 0, fontColor);
			if (menuSelection == 2)
				fontColor = D3DCOLOR_ARGB(255,240,100,100); 
			else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
			drawTextAndNumber(TEXT("Speed Modifier: "), speedMod, speed, fontColor);
			if (menuSelection == 3)
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
			judge(currentRating);
			drawTextAndNumber(TEXT("BPM: "), bpm, topDisplay1, fontColor);
			drawTextAndNumber(TEXT("Speed Modifier: "), speedMod, topDisplay2, fontColor);
			/*drawTextAndNumber(TEXT("diff "), diff, bottom, fontColor);
			drawTextAndNumber(TEXT("cur "), values[index-1], start, fontColor);
			drawTextAndNumber(TEXT("old "), values[index-2], quit, fontColor);*/
			gameSprites->End();
			waitTime--;
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
	D3DXCreateFont(pDev, 30, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"), &font); 
	setRects();
	index = 2;
	diff = 0;
	screen = 0;
	speedMod = 3.5;
	menuSelection = 0;
	score = 0;
}

void Game::initGameScreen() {
	loadTextures();	
	D3DXCreateFont(pDev, 30, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic Demi"), &font); 
	srand((unsigned int)(time(0)));
	fontColor2 = D3DCOLOR_ARGB(255,240,240,240);
	curFrame = 0;
	curRow = 0;
	curAlpha = 50;
	currentT = 0;
	hits = 0;
	ratingTime = 0;

	cols[0] = 20;
	cols[1] = 120;
	cols[2] = 220;
	cols[3] = 320;

	placeObject(&stepZone, cols[0], BEATDIST, 0, target, 0, 1);
	placeObject(&stepZone, cols[1], BEATDIST, 0, target, 0, 1);
	placeObject(&stepZone, cols[2], BEATDIST, 0, target, 0, 1);
	placeObject(&stepZone, cols[3], BEATDIST, 0, target, 0, 1);
	lastCol = -1;
	/*setArrows(9,10,SIXTEENTH);
	setArrows(10,11,SIXTEENTH);
	setArrows(12,13,SIXTEENTH);
	setArrows(13,14,SIXTEENTH);
	setArrows(17,18,SIXTEENTH);
	setArrows(21,22,SIXTEENTH);
	setArrows(25,26,SIXTEENTH);*/
	//setArrows(7,10,SIXTEENTH);
	/*placeObject(&arrows, 7, cols[0], arrow, 1, 1);
	placeObject(&arrows, 7.25, cols[1], arrow, 1, 1);
	placeObject(&arrows, 7.5, cols[2], arrow, 1, 1);
	placeObject(&arrows, 7.75, cols[3], arrow, 1, 1);*/
	setArrows(9,13,QUARTER);
	setArrows(13,17.5,EIGHTH);
	setArrows(18,21,QUARTER);
	setArrows(21,25.5,EIGHTH);
	setArrows(26,29,QUARTER);
	setArrows(29,33.5,EIGHTH);
	setArrows(34,37,QUARTER);
	setArrows(37,41,EIGHTH);
	setArrows(41,44,QUARTER);
	setArrows(44,45.5,SIXTEENTH);
	setArrows(46,47.5,SIXTEENTH);
	setArrows(48,52,QUARTER);
	setArrows(52,53.5,SIXTEENTH);
	setArrows(54,55.5,SIXTEENTH);
	setArrows(56,60,QUARTER);
	setArrows(60,61.5,SIXTEENTH);
	setArrows(62,63.5,SIXTEENTH);
	setArrows(64,70,QUARTER);

	values[0] = arrows[1].getPos(1);
	values[1] = arrows[1].getPos(1);

	IGraphBuilder *pGraph;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	if (song == 0)
		hr = pGraph->RenderFile(TEXT("rem3.mp3"), NULL);
	if (song == 1)
		hr = pGraph->RenderFile(TEXT("dest.mp3"), NULL);
	hr = pControl->Run();
}

void Game::setRects() {
	start1.left=SCREEN_WIDTH*1/6;
	start1.right=SCREEN_WIDTH;
	start1.top=SCREEN_HEIGHT*2/5;
	start1.bottom=start1.top+45;
	start2.left=start1.left;
	start2.right=start1.right;
	start2.top=start1.bottom+30;
	start2.bottom=start2.top+45;
	speed.left=start2.left;
	speed.right=start2.right;
	speed.top=start2.bottom+30;
	speed.bottom=speed.top+45;
	quit.left=speed.left;
	quit.right=speed.right;
	quit.top=speed.bottom+30;
	quit.bottom=quit.top+45;
	topDisplay1.left = 550;
	topDisplay1.right = 800;
	topDisplay1.top = 5;
	topDisplay1.bottom = 65;
	topDisplay2.left = 550;
	topDisplay2.right = 800;
	topDisplay2.top = 67;
	topDisplay2.bottom = 135;
	ratingDisplay.left = 500;
	ratingDisplay.right = 700;
	ratingDisplay.top = 250;
	ratingDisplay.bottom = 320;
	arrow.left=0;
	arrow.top=1050;
	arrow.right=55;
	arrow.bottom=1070;
	explosion.left = 333;
	explosion.top = 1988;
	explosion.right = 380;
	explosion.bottom = 2056;
	target.left = 282;
	target.top = 1980;
	target.right = 322;
	target.bottom = 2000;
	bottom.left=quit.left;
	bottom.right=quit.right;
	bottom.top=quit.bottom+30;
	bottom.bottom=bottom.top+45;
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
		if (arrows[i].isActive()) {
			if (arrows[i].isExploding() && arrows[i].getPos(1) > stepZone[0].getPos(1)-30) {
				for (size_t j = 0; j < 4; j++) {
					if (arrows[i].getPos(0) == cols[j])
						gameSprites->Draw(targetTexture, &explosion, NULL, &stepZone[j].getPos(), 0xFFFFFFFF);
				}
				arrows[i].setAnimTime(arrows[i].getAnimTime()-1);
				if (arrows[i].getAnimTime() <= 0) {
					arrows[i].setActive(false);
				}
			}
			else {
				if (arrows[i].getType() == QUARTER) 
					gameSprites->Draw(targetTexture, &arrow, NULL, &arrows[i].getPos(), D3DCOLOR_ARGB(255,255,125,125));
				else if (arrows[i].getType() == EIGHTH) 
					gameSprites->Draw(targetTexture, &arrow, NULL, &arrows[i].getPos(), D3DCOLOR_ARGB(255,125,125,255));
				else if (arrows[i].getType() == SIXTEENTH)
					gameSprites->Draw(targetTexture, &arrow, NULL, &arrows[i].getPos(), D3DCOLOR_ARGB(255,125,255,125));
			}
		}
	}
}

void Game::moveObjects() {
	for (size_t i = 0; i < arrows.size(); i++) {
		if (!arrows[i].isExploding() || abs(arrows[i].getPos(1)-stepZone[0].getPos(1)) >= 30) {	
			arrows[i].move(0,-2*speedMod,0);
			if (arrows[i].getPos(1) < stepZone[0].getPos(1)-30 && !arrows[i].isExploding()) {
				currentRating = 100;
				ratingTime = 20;
				arrows[i].setExploding(true);
			}
			if (arrows[i].getPos(1) < -10)
				arrows[i].setActive(false);
		}
	}
}

void Game::placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type, float speed) {
	GameObject next;
	next.init(x,y,z,bounds,type,speed);
	vec->push_back(next);
}

void Game::placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type, float speed) {
	GameObject next;
	float nextType = beat - (int)beat;
		if (nextType == 0)
			nextType = QUARTER;
		else if (fmodf(nextType, 0.5) == 0)
			nextType = EIGHTH;
		else if (fmodf(nextType, 0.25) == 0)
			nextType = SIXTEENTH;
	next.init(column,BEATDIST*(float(170.0f)/float(bpm))*beat*speedMod,0,bounds,nextType,speed);
	vec->push_back(next);
}

void Game::setArrows(float start, float end, float type) {
	for (float i = start; i < end; i+=type) {
		int nextCol = cols[rand()%4];
		while (nextCol == lastCol)
			nextCol = cols[rand()%4];
		lastCol = nextCol;
		float nextType = i - (int)i;
		if (nextType == 0)
			nextType = QUARTER;
		else if (fmodf(nextType, 0.5) == 0)
			nextType = EIGHTH;
		else if (fmodf(nextType, 0.25) == 0)
			nextType = SIXTEENTH;
		placeObject(&arrows, i, nextCol, arrow, nextType, 1);
	}
}

void Game::judge(float distance) {
	if (ratingTime > 0) {
		if (distance <= 10)
			font->DrawText(NULL, TEXT("Fantastic!"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,240,240,255));
		else if (distance <= 20)
			font->DrawText(NULL, TEXT("Excellent"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,240,240,100));
		else if (distance <= 30)
			font->DrawText(NULL, TEXT("Decent"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,125,240,125));
		else if (distance > 30)
			font->DrawText(NULL, TEXT("Miss"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,240,125,125));
	}
	ratingTime--;
}

/* void drawTextAndNumber(LPCWSTR text, int num, RECT pos, D3DCOLOR fontColor): draws text with a variable number after it.
        Parameters:
        LPCWSTR text: the text before the number.
        int num: the number value.
        RECT pos: where to draw the text.
        D3DCOLOR fontColor: the colour of the text. */

void Game::drawTextAndNumber(LPCWSTR text, float num, RECT pos, D3DCOLOR fontColor) {
        /*wchar_t buffer[16];
        wsprintf(buffer, TEXT("%d"), num);
        wstring s(text);
        s += wstring(buffer);*/ 
		wstring s(text);
		wstringstream wss;
		wss << text << num;
        font->DrawText(NULL, wss.str().c_str(), -1, &pos, 0, fontColor);
}

/* void resetMatrices(): returns all transformation matrices to identity matrices. */
void Game::resetMatrices() {
	D3DXMatrixIdentity(&spriteManip);
	D3DXMatrixIdentity(&rotation);
	D3DXMatrixIdentity(&scaling);
	D3DXMatrixIdentity(&translation1);
	D3DXMatrixIdentity(&translation2);
}