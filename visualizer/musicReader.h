/* musicReader.h
Handles the playing of sound files and extraction of audio sample data from them.  DirectShow-related functions are dealt with by this class. */

#ifndef MR_H
#define MR_H

#include <DShow.h>
#include <assert.h>
#include "qedit.h"
#include "grabberCb.h"
#include <vector>

class MusicReader {

private:
	// objects for building filter graph
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;

	// objects for adding the sample grabber
	IBaseFilter *pGrabberF;
    ISampleGrabber *pGrabber;
	IBaseFilter *pNullF;
	AM_MEDIA_TYPE mt;

	HWND hwnd;
	int song;
	bool grabbed;
	CGrabCB grabCB;

public:
	void init(HWND hwnd_, int song_, vector<BYTE>* drawValue);
	void play();
	void stop();
	void pause();
	void setupSampleGrabber();
	void getSample();
	bool isGrabbed();
	bool isReady();
	long getSampleSize();
	HRESULT IsPinConnected(IPin *pPin, BOOL *pResult);
	HRESULT IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult);
	HRESULT MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult);
	HRESULT FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
};

#endif