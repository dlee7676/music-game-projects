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
	d3dpp.Windowed = FALSE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDev);

	// set game parameters
	initMenuScreen();
	displayMode = 3.5;
	screen = 0;
	menuSelection = 0;
	displayMode = 0;
	drawLast = false;
	refresh = 0;
	setRects();
}

/* void gameloop(): function run repeatedly in the message loop, executes drawing code and responds to game controls */
void Game::gameloop() {
	render();
	handleInput();
}

/* void handleInput(): responds to key presses of the game controls */
void Game::handleInput() {
	// Esc returns to the menu screen from gameplay
	if (GetAsyncKeyState(VK_ESCAPE) && screen != 0) {
		screen = 0;
		initMenuScreen();
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
				if (menuSelection == 2) {
					if (!displayMode)
						displayMode = 1;
					else displayMode--;
				}
				Sleep(150);
			}
			if (GetAsyncKeyState(VK_RIGHT)) {
				if (menuSelection == 2) {
					if (displayMode)
						displayMode = 0;
					else displayMode++;
				}
				Sleep(150);
			}
			// select an option
			if (GetAsyncKeyState(VK_RETURN)) {
				if (menuSelection == 0) {
					screen = 1;
					song = 0;
					initGameScreen();
				}
				if (menuSelection == 1) {
					screen = 1;
					song = 1;
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
	}
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
			font->DrawText(NULL, TEXT("Play Arabesque No. 1"), -1, &start1, 0, fontColor);
			toggleMenuText(1);
			font->DrawText(NULL, TEXT("Play Destination Unknown"), -1, &start2, 0, fontColor);
			toggleMenuText(2);
			if (!displayMode)
				font->DrawText(NULL, TEXT("Display mode: time"), -1, &mode, 0, fontColor);
			else font->DrawText(NULL, TEXT("Display mode: frequency"), -1, &mode, 0, fontColor);
			toggleMenuText(3);
			font->DrawText(NULL, TEXT("Quit"), -1, &quit, 0, fontColor);

			break;
		}
		case 1: {
			// draw the game elements
			gameSprites->Begin(D3DXSPRITE_ALPHABLEND);
			drawSamples();
			gameSprites->End();
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
	time = 0;
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
	grabbed = false;
	run = false;
	pitch = 0;
	playMusic(song);	
}

/* void loadTextures(int level_): loads the textures needed for a level. */
void Game::loadTextures() {
	if (FAILED(D3DXCreateTextureFromFile(pDev, TEXT("note.png"), &targetTexture))) {
		MessageBox(hwnd, TEXT("Error Loading Texture"), TEXT("Error"), MB_ICONERROR);
		return;
	}
}

/* void drawSamples(): draws either a time or frequency spectrum graph of the current sample data from the sound file. */
void Game::drawSamples() {
	time++;
	int n = sampleBuffer.size();
	float current, realValue;
	bool powerOfTwo = !(n == 0) && !(n & (n - 1));
	if (!powerOfTwo) {
		n = upper_power_of_two(n)/2;
	}

	int width;
	if (!displayMode)
		width = 5;
	else width = 4;

	cx_vec timeValues(8192);
	cx_vec frequencyValues(8192);
	cx_vec display(8192);
	// convert byte values from the sound file into floats and add them to a container of samples for the time spectrum
	if (musicReader.isReady() && !sampleBuffer.empty() && refresh <= 0) {
		drawLast = false;
		grabbed = true;
		for (size_t i = 0; i < n; i++) {
			if (n > 0 && musicReader.isReady() && !sampleBuffer.empty()) {
				short sample = sampleBuffer[i+1];
				sample = (sample << 8) + sampleBuffer[i];
				float input = sample/32768.0f;
				timeValues(i) = input;
				int b = 0;
			}
		}
		// Fourier transform the sample values to get values for the frequency spectrum
		frequencyValues = fft(timeValues);
		int max = 0;
		int index = 1;
		for (size_t f = 0; f < 400; f++) {
			if (real(frequencyValues(f)) > max) {
				max = real(frequencyValues(f));
				index = f;
			}
		}

		for (size_t i = 0, j = 0, f = 1; i < n; i+=sampleBuffer.size()/300, j+=width, f++) {
			if (i < sampleBuffer.size()) {
				/* find whether the highest frequency value (representing the strength of that frequency) is in the low, middle, or high range; this will determine
				   the colour used to draw the graph */
				// bass
				if (index < 5)
					pitch = 0;
				// mid-range
				else if (index > 5 && index < 50)
					pitch = 1;
				// treble
				else if (index > 50 && index < 250)
					pitch = 2;
				// high frequencies
				else pitch = 3;

				// choose whether time or frequency spectrum will be displayed
				if (!displayMode) {
					current = -1*real(timeValues(i))*250+255;
					last = timeValues;
				}
				else {
					current = -3*abs(real(frequencyValues(i)))+500;
					last = frequencyValues;
				}

				// draw the time or frequency graph with the appropriate colour
				if(musicReader.isReady() && !sampleBuffer.empty()) {
					switch(pitch) {
						case 0: {
							// yellow = bass
							pitchColor = D3DCOLOR_ARGB(255, 200, 255, 50);
							break;
						}
						case 1: {
							// green = mid-range
							pitchColor = D3DCOLOR_ARGB(255, 50, 255, 50);
							break;
						}
						case 2: {
							// cyan = treble
							pitchColor = D3DCOLOR_ARGB(255, 50, 255, 200);
							break;
						}
						case 3: {
							// blue = high frequencies
							pitchColor = D3DCOLOR_ARGB(255, 50, 150, 200);
							break;								
						}
						default: {	
							pitchColor = D3DCOLOR_ARGB(100, 255, 255, 255);
							break;
						}
					}
					if (displayMode) {
						for (int y = 0; y < current; y+=3) {
							current = abs(real(frequencyValues(i)));
							gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((float)j, 500-(y*3), 0), D3DCOLOR_ARGB(250,250,250,250));
						}
						gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((float)j, -3*current+500, 0), pitchColor);
					}
					else gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((float)j, current, 0), pitchColor);
				}
			}
		}
	}
	// attempting to reduce screen flickering by also drawing previous frame when in the frequency graph 
	if (grabbed) {
		for (size_t i = 0, j = 0, f = 1; i < n; i+=sampleBuffer.size()/300, j+=width, f++) {
			if (displayMode) {
				int a = 0;
				current = -3*abs(real(last(i)))+500;
				for (int y = 0; y < current; y+=3) {
					current = abs(real(last(i)));
					gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((float)j, 500-(y*3), 0), D3DCOLOR_ARGB(100,240,240,240));
				}
				gameSprites->Draw(targetTexture, &explosion, NULL, &D3DXVECTOR3((float)j, -3*current+500, 0), pitchColor);
			}
		}
	}
}

/* void playMusic(int song_): plays a specified music file and collects sample data from it. 
	Parameters:
	int song: indicates the chosen song. */
void Game::playMusic(int song_) {
	musicReader.init(hwnd, song_, &sampleBuffer);
	musicReader.setupSampleGrabber();
	musicReader.getSample();
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
	mode.left=start2.left;
	mode.right=start2.right;
	mode.top=start2.bottom+30;
	mode.bottom=mode.top+45;
	quit.left=mode.left;
	quit.right=mode.right;
	quit.top=mode.bottom+30;
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
	explosion.left = -25;
	explosion.top = -25;
	explosion.right = 50;
	explosion.bottom = 50;
}

// function for obtaining the nearest upper power of two to a number; from http://graphics.stanford.edu/~seander/bithacks.html
unsigned long Game::upper_power_of_two(unsigned long v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;

}
