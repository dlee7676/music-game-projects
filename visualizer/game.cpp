#include "game.h"

/* void setHwnd(): used to bring the window handle from the main function into the Game object for initializing Direct3D */
void Game::setHwnd(HWND _hwnd) {
	hwnd = _hwnd;
}

/* void initGame(): initializes Direct3D and DirectShow and sets initial game state */
void Game::initGame() {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(FAILED(d3d)) {
		MessageBox(hwnd, TEXT("Error initializing D3D object"), TEXT("Error"), MB_ICONERROR);
	}
	D3DPRESENT_PARAMETERS d3dpp;
	// sets the presentation parameters
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDev);

	// initalize DirectShow for playing sound
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error initializing DirectShow"), TEXT("Error"), MB_ICONERROR);
	}

	// set game parameters
	initMenuScreen();
	speedMod = 3.5;
	screen = 0;
	menuSelection = 0;
	setRects();
}

/* void gameloop(): function run repeatedly in the message loop, executes drawing code and responds to game controls */
void Game::gameloop() {
	moveObjects();
	render();
	handleInput();
}

/* void handleInput(): responds to key presses of the game controls */
void Game::handleInput() {
	// Esc returns to the menu screen from gameplay
	if (GetAsyncKeyState(VK_ESCAPE) && screen != 0) {
		screen = 0;
		initMenuScreen();
		stepZone.clear();
		notes.clear();
		sampleBuffer.clear();
		targetTexture->Release();
		musicReader.stop();
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
				}
				if (menuSelection == 1) {
					screen = 1;
					song = 1;
					bpm = 75;
					initGameScreen();
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
			// D, F, J, and K attempt to hit notes in the four columns
			if (GetAsyncKeyState('D') && waitTime[1] <= 0) {
				hitNote(0);
			}
			if (GetAsyncKeyState('F') && waitTime[2] <= 0) {
				hitNote(1);
			}
			if (GetAsyncKeyState('J') && waitTime[3] <= 0) {
				hitNote(2);
			}
			if (GetAsyncKeyState('K') && waitTime[4] <= 0) {
				hitNote(3);
			}
			if (GetAsyncKeyState('G') && waitTime[0] <= 0) {
				changeSpeed(0.25, 0);
			}
			if (GetAsyncKeyState('H') && waitTime[0] <= 0) {
				changeSpeed(0.25, 1);
			}
			break;
		}
	}
}

/* void hitNote(int column): Interacts with a note in a given column when it is within a certain range of the targets.  Each column has a 
	corresponding key that calls this when it is pressed.
	Parameters:
	int column: the column to check for notes. */
void Game::hitNote(int column) {
	// check each note in the chart to see if it is in the column of the pressed key, if it has been hit yet, and if it is close enough to the targets
	for (size_t i = 0; i < notes.size(); i++) {
		if (!notes[i].isHit() && notes[i].getPos(0) == cols[column] && abs(stepZone[0].getPos(1) + 5 - notes[i].getPos(1)) <= 40) {
			notes[i].setHit(true);
			// get the distance from the note to the target for judging the timing
			currentRating = abs(stepZone[0].getPos(1) + 5 - notes[i].getPos(1));
			ratingTime = 40;
			// add a short delay before this can be triggered again so that this only occurs once per key press
			waitTime[column+1]=10;
			// exit the loop so that only one note is hit at a time
			break;
		}
	}
}

/* void changeSpeed(float difference, int direction): Increases or decreases the scroll speed of notes, and adjusts their spacing to make them reach the targets
	at the same time as they would at the previous speed.
	Parameters:
	float difference: the amount to change the speed by.
	int direction: 0 to decrease speed, nonzero to increase. */
void Game::changeSpeed(float difference, int direction) {
	float lastMod = speedMod;
	if (direction == 0) {
		if (speedMod > difference) 
			speedMod-=difference;
	}
	else speedMod+=difference;
	for(size_t i = 0; i < notes.size(); i++) 
		if (notes[i].isActive())
			notes[i].setPos(notes[i].getPos(0), (notes[i].getPos(1)-stepZone[0].getPos(1))*(speedMod/lastMod)+stepZone[0].getPos(1), 0);
	waitTime[0]=10;
}


/* void render(): determines the positions of game objects and draws them on the screen. */
void Game::render() {
	pDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	pDev->BeginScene();
	switch(screen) {
		case 0: {
			// draw menu screen
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			gameSprites->End();
			toggleMenuText(0);
			font->DrawText(NULL, TEXT("Play REM III (BPM 170)"), -1, &start1, 0, fontColor);
			toggleMenuText(1);
			font->DrawText(NULL, TEXT("Play Destination Unknown (BPM 75)"), -1, &start2, 0, fontColor);
			toggleMenuText(2);
			drawTextAndNumber(TEXT("Speed Modifier: "), speedMod, speed, fontColor);
			toggleMenuText(3);
			font->DrawText(NULL, TEXT("Quit"), -1, &quit, 0, fontColor);

			break;
		}
		case 1: {
			// draw the game elements
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			if (musicReader.isGrabbed()) {
				/* line mode
				time++;
				for (int i = 0; i < sampleBuffer.size(); i++) {
					gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((i*10-20-time), -1*sampleBuffer[i]+400, 0), D3DCOLOR_ARGB(255, 255, 255, 255));
					if (i*10-20-time*1.5 > SCREEN_WIDTH || i*10-20-time*1.5 < 0)
						break;
				}*/
				int modifier = (rand()%100)-50;
				float* convertedBuffer;
				if (musicReader.isReady())
					 //convertedBuffer = convert16BitToFloat(sampleBuffer);

				int a = 0;
				for (int i = 0; i < sampleBuffer.size()/2-2; i+=2) { 
					time++;
					if (!sampleBuffer.empty() && musicReader.isReady() && i%6==0) {
						//if (-1*sampleBuffer[i]+375 > 250)
						short sample = 0;
						sample = (sample << 8) + sampleBuffer[i+1];
						sample = (sample << 8) + sampleBuffer[i];
						float input = sample/32768.0f;
						gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((i*10%SCREEN_WIDTH-20), -1*input*250+280, 0), D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}
			}
			gameSprites->End();
			for (int i = 0; i < 5; i++)
				waitTime[i]--;
			break;
		}
	}
	pDev->EndScene();
	pDev->Present(NULL, NULL, NULL, NULL);
}

/* void cleanup(): frees the memory associated with the game's objects. */
void Game::cleanup() {
	d3d->Release();
	pDev->Release();
	gameSprites->Release();
	font->Release();
}

/* void initMenuScreen(): initializes values relevant to the menu screen. */
void Game::initMenuScreen() {
	if (FAILED(D3DXCreateSprite(pDev, &gameSprites))) {
		MessageBox(hwnd, TEXT("Error Loading Sprite"), TEXT("Error"), MB_ICONERROR);
		return;
	} 
	D3DXCreateFont(pDev, 30, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"), &font); 
}

/* void drawTextAndNumber(LPCWSTR text, int num, RECT pos, D3DCOLOR fontColor): draws text with a variable number after it.
        Parameters:
        LPCWSTR text: the text before the number.
        int num: the number value.
        RECT pos: where to draw the text.
        D3DCOLOR fontColor: the colour of the text. */
void Game::drawTextAndNumber(LPCWSTR text, float num, RECT pos, D3DCOLOR fontColor) {
		wstring s(text);
		wstringstream wss;
		wss << text << num;
        font->DrawText(NULL, wss.str().c_str(), -1, &pos, 0, fontColor);
}

/* void toggleMenuText(int selection): highlights the chosen menu selection. 
	Parameters:
	int selection: the currently selected menu option. */
void Game::toggleMenuText(int selection) {
	if (menuSelection == selection)
		fontColor = D3DCOLOR_ARGB(255,240,100,100); 
	else fontColor = D3DCOLOR_ARGB(255,240,240,240); 
}

/* void initGameScreen(): loads textures for game elements and initializes values relevant to the play screen. */
void Game::initGameScreen() {
	loadTextures();	
	D3DXCreateFont(pDev, 30, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Franklin Gothic Demi"), &font); 
	// set the positions of the target areas at the top
	cols[0] = 20;
	cols[1] = 120;
	cols[2] = 220;
	cols[3] = 320;
	ratingTime = 0;
	time = 0;
	grabbed = false;
	playMusic(song);	
}

/* void loadTextures(int level_): loads the textures needed for a level. */
void Game::loadTextures() {
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("note.png"), &targetTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
}

/* void drawStepZone(): draws the target area at the top of the screen. */
void Game::drawStepZone() {
	for (size_t i = 0; i < stepZone.size(); i++) {
		index = i;
		if (stepZone[i].getType()==0)
			gameSprites->Draw(targetTexture, &stepZone[i].getDrawingBounds(), NULL, &D3DXVECTOR3(stepZone[i].getPos(0),stepZone[i].getPos(1)+10, 0), 0xFFFFFFFF);
	}
}

/* void drawNotes(): draws the scrolling notes and the "explosion" when they are hit. */
void Game::drawNotes() {
	for (size_t i = 0; i < notes.size(); i++) {
		if (notes[i].isActive()) {
			// draw an explosion if the note has been hit (the key for its column was pressed when it was close to the targets)
			if (notes[i].isHit() && notes[i].getPos(1) > stepZone[0].getPos(1)-40) {
				for (int j = 0; j < 4; j++) {
					if (notes[i].getPos(0) == cols[j])
						gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3(stepZone[j].getPos(0)-10,stepZone[j].getPos(1)-17, 0), 0xFFFFFFFF);
				}
				// show the explosion for a given amount of time and then set the note to be inactive
				notes[i].setAnimTime(notes[i].getAnimTime()-1);
				if (notes[i].getAnimTime() <= 0) {
					notes[i].setActive(false);
				}
			}
			// draw the unhit notes with colours corresponding to their timing
			else {
				/*if (notes[i].getType() == QUARTER) 
					gameSprites->Draw(targetTexture, &notes[i].getDrawingBounds(), NULL, &notes[i].getPos(), D3DCOLOR_ARGB(255,255,60,125));
				else if (notes[i].getType() == EIGHTH) 
					gameSprites->Draw(targetTexture, &notes[i].getDrawingBounds(), NULL, &notes[i].getPos(), D3DCOLOR_ARGB(255,125,60,255));
				else if (notes[i].getType() == SIXTEENTH)
					gameSprites->Draw(targetTexture, &notes[i].getDrawingBounds(), NULL, &notes[i].getPos(), D3DCOLOR_ARGB(255,125,255,125));
				else if (notes[i].getType() == THIRTYSECOND)
					gameSprites->Draw(targetTexture, &notes[i].getDrawingBounds(), NULL, &notes[i].getPos(), D3DCOLOR_ARGB(255,250,75,50));*/
				gameSprites->Draw(targetTexture, &notes[i].getDrawingBounds(), NULL, &notes[i].getPos(), D3DCOLOR_ARGB(255,250,50,250));
			}
		}
	}
}

/* void playMusic(int song_): plays a specified music file. 
	Parameters:
	int song: indicates the chosen song. */
void Game::playMusic(int song_) {
	musicReader.init(hwnd, song_, &sampleBuffer);
	musicReader.setupSampleGrabber();
	musicReader.getSample();
}

/* void drawNotes(): draws the scrolling notes and the "explosion" when they are hit. */
void Game::moveObjects() {
	for (size_t i = 0; i < notes.size(); i++) {
		if (!notes[i].isHit() || abs(notes[i].getPos(1)-stepZone[0].getPos(1)) >= 40) {	
			notes[i].move(0,-2*speedMod*(float(bpm)/float(170.0f)),0);
			if (notes[i].getPos(1) < stepZone[0].getPos(1)-40 && !notes[i].isHit()) {
				currentRating = 100;
				ratingTime = 40;
				notes[i].setHit(true);
			}
			if (notes[i].getPos(1) < -10)
				notes[i].setActive(false);
		}
	}
}

/* void judge(float distance): displays a rating for when a note is hit, judging how accurate the timing was. 
	Parameters:
	float distance: the note's distance from the target zone when it was hit; 0 is perfectly timed. */ 
void Game::judge(float distance) {
	// display text for a given amount of time after a hit
	if (ratingTime > 0) {
		if (distance <= 10)
			font->DrawText(NULL, TEXT("Fantastic!"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,240,240,255));
		else if (distance <= 20)
			font->DrawText(NULL, TEXT("Excellent"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,240,240,100));
		else if (distance <= 40)
			font->DrawText(NULL, TEXT("Decent"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,125,240,125));
		else if (distance > 40)
			font->DrawText(NULL, TEXT("Miss"), -1, &ratingDisplay, 0, D3DCOLOR_ARGB(240,240,125,125));
	}
	ratingTime--;
}

/* void setRects(): defines rectangles for drawing game graphics. */
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
	explosion.left = 0;
	explosion.top = 0;
	explosion.right = 60;
	explosion.bottom = 60;
}

float* Game::convert16BitToFloat(vector<BYTE> samples) {
	int inputSamples = samples.size()/2;
	float* output = new float[inputSamples];
	int outputIndex = 0;
	BYTE* input = &samples[0];
		
	for (int i = 0; i < inputSamples; i+=2) {
		//BitConverter.ToInt16(input,i*2);
		short sample = 0;
		sample = (sample << 8) + samples[i];
		sample = (sample << 8) + samples[i+1];
		output[outputIndex] = sample/32768.0f;
		outputIndex++;
	}
	int a = 0;
	return output;
}
