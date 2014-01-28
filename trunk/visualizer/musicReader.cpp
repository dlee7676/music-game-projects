#include "musicReader.h"

using namespace std;

/* void init(float x, float y, float z, RECT bounds_, int type_, float speed_):
	Initializes a MusicReader object with the following characteristics:
	Parameters:
	HWND hwnd_: handle to the program window.
	int song_: the song to play.  
	vector<BYTE>* drawValue: pointer to a vector of BYTE values, to be used for drawing the waveform. */
void MusicReader::init(HWND hwnd_, int song_, vector<BYTE>* drawValue) {
	hwnd = hwnd_;
	song = song_;
	grabbed = false;
	grabCB.setDrawValue(drawValue);

	// initialize COM library for DirectShow
	HRESULT hr = CoInitialize(NULL);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error initializing DirectShow"), TEXT("Error"), MB_ICONERROR);
	}

	// set up a DirectShow FilterGraph to play the music file
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error creating filter graph"), TEXT("Error"), MB_ICONERROR);
	}
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error starting graph controller"), TEXT("Error"), MB_ICONERROR);
	}
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	if(FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error starting graph event handler"), TEXT("Error"), MB_ICONERROR);
	}
}

// play, pause, or stop audio
void MusicReader::play() {
	// play the audio
	HRESULT hr = pControl->Run();
	if (FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error playing audio"), TEXT("Error"), MB_ICONERROR);;
	}
}
void MusicReader::pause() {
	HRESULT hr = pControl->Pause();
	if (FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error stopping audio"), TEXT("Error"), MB_ICONERROR);;
	}
}
void MusicReader::stop() {
	HRESULT hr = pControl->Stop();
	if (FAILED(hr)) {
		MessageBox(hwnd, TEXT("Error stopping audio"), TEXT("Error"), MB_ICONERROR);;
	}
}


/* void setupSampleGrabber(): Creates a DirectShow filter graph, adds a sample grabber to it, and configures the grabber.
	Based on code from http://www.geekpage.jp/en/programming/directshow/samplegrabber-1.php */
void MusicReader::setupSampleGrabber() {
	// Create the Sample Grabber filter.
    HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pGrabberF));
    if (FAILED(hr))
    {
        MessageBox(hwnd, TEXT("Error creating sample grabber"), TEXT("Error"), MB_ICONERROR);
    }

    hr = pGrabberF->QueryInterface(IID_PPV_ARGS(&pGrabber));
    if (FAILED(hr))
    {
        MessageBox(hwnd, TEXT("Error querying sample grabber filter"), TEXT("Error"), MB_ICONERROR);
    }
	// determine the format for connecting SampleGrabber.
	ZeroMemory(&mt, sizeof(mt));
	mt.majortype = MEDIATYPE_Audio;
	mt.subtype = MEDIASUBTYPE_PCM;
	hr = pGrabber->SetMediaType(&mt);
	if (FAILED(hr))
	{
		MessageBox(hwnd, TEXT("Error setting media type"), TEXT("Error"), MB_ICONERROR);
	}
	// add SampleGrabber Filter to the Graph
	hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr))
    {
        MessageBox(hwnd, TEXT("Error adding grabber to filter graph"), TEXT("Error"), MB_ICONERROR);
    }
	// get MediaControl
	pGraph->QueryInterface(IID_IMediaControl,
		(LPVOID *)&pControl);
	// create Graph.
	// Graph that contains SampleGrabber
	// will be created automatically.
	if (!grabbed) {
		if (song == 0)
			pGraph->RenderFile(TEXT("arabesque.mp3"), NULL);
		if (song == 1)
			pGraph->RenderFile(TEXT("dest.mp3"), NULL);
	}
	// Get connection information.
	// This must be done after the Graph is created
	// by RenderFile.
	pGrabber->GetConnectedMediaType(&mt);
	// Set SampleGrabber to not automatically generate the sample buffer (this will be done by a callback function instead)
	pGrabber->SetBufferSamples(FALSE);
	// play audio file if no samples have been grabbed
	if (!grabbed)
		play();
}

void MusicReader::getSample() {
	// Set the SampleGrabber to use the grabCB callback function on the audio data it receives
	HRESULT hr = pGrabber->SetCallback(&grabCB, 1);
	if (FAILED(hr))
    {
        MessageBox(hwnd, TEXT("Error using callback"), TEXT("Error"), MB_ICONERROR);
    }
	grabbed = true;
}

// check if a sample has been obtained yet
bool MusicReader::isGrabbed() {
	return grabbed;
}
// check if the sample buffer is ready to be read
bool MusicReader::isReady() {
	return grabCB.isReady();
}


/* ----------------------------------------- */

/* Filter connecting functions from MSDN; not currently used, but may be needed if the filter graph needs to be customized further */

// Query whether a pin is connected to another pin.
//
// Note: This function does not return a pointer to the connected pin.
HRESULT MusicReader::IsPinConnected(IPin *pPin, BOOL *pResult)
{
    IPin *pTmp = NULL;
    HRESULT hr = pPin->ConnectedTo(&pTmp);
    if (SUCCEEDED(hr))
    {
        *pResult = TRUE;
    }
    else if (hr == VFW_E_NOT_CONNECTED)
    {
        // The pin is not connected. This is not an error for our purposes.
        *pResult = FALSE;
        hr = S_OK;
    }

    //pTmp->Release();
    return hr;
}

// Query whether a pin has a specified direction (input / output)
HRESULT MusicReader::IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult)
{
    PIN_DIRECTION pinDir;
    HRESULT hr = pPin->QueryDirection(&pinDir);
    if (SUCCEEDED(hr))
    {
        *pResult = (pinDir == dir);
    }
    return hr;
}

// Match a pin by pin direction and connection state.
HRESULT MusicReader::MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult)
{
    assert(pResult != NULL);

    BOOL bMatch = FALSE;
    BOOL bIsConnected = FALSE;

    HRESULT hr = IsPinConnected(pPin, &bIsConnected);
    if (SUCCEEDED(hr))
    {
        if (bIsConnected == bShouldBeConnected)
        {
            hr = IsPinDirection(pPin, direction, &bMatch);
        }
    }

    if (SUCCEEDED(hr))
    {
        *pResult = bMatch;
    }
    return hr;
}

// Return the first unconnected input pin or output pin.
HRESULT MusicReader::FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
    IEnumPins *pEnum = NULL;
    IPin *pPin = NULL;
    BOOL bFound = FALSE;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        goto done;
    }


    while (S_OK == pEnum->Next(1, &pPin, NULL))
    {
        hr = MatchPin(pPin, PinDir, FALSE, &bFound);
        if (FAILED(hr))
        {
            goto done;
        }
        if (bFound)
        {
            *ppPin = pPin;
            (*ppPin)->AddRef();
            break;
        }
        //pPin->Release();
    }

    if (!bFound)
    {
        hr = VFW_E_NOT_FOUND;
    }

done:
    //pPin->Release();
    //pEnum->Release();
    return hr;
}

// Connect output pin to filter.

HRESULT MusicReader::ConnectFilters(
    IGraphBuilder *pGraph, // Filter Graph Manager.
    IPin *pOut,            // Output pin on the upstream filter.
    IBaseFilter *pDest)    // Downstream filter.
{
    IPin *pIn = NULL;
        
    // Find an input pin on the downstream filter.
    HRESULT hr = FindUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (SUCCEEDED(hr))
    {
        // Try to connect them.
        hr = pGraph->Connect(pOut, pIn);
        pIn->Release();
    }
    return hr;
}

// Connect filter to input pin.

HRESULT MusicReader::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn)
{
    IPin *pOut = NULL;
        
    // Find an output pin on the upstream filter.
    HRESULT hr = FindUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (SUCCEEDED(hr))
    {
        // Try to connect them.
        hr = pGraph->Connect(pOut, pIn);
        pOut->Release();
    }
    return hr;
}

// Connect filter to filter

HRESULT MusicReader::ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
    IPin *pOut = NULL;

    // Find an output pin on the first filter.
    HRESULT hr = FindUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (SUCCEEDED(hr))
    {
        hr = ConnectFilters(pGraph, pOut, pDest);
        pOut->Release();
    }
    return hr;
}