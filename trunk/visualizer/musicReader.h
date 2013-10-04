#ifndef MR_H
#define MR_H

#include <DShow.h>
#include <assert.h>
#include "qedit.h"
#include "grabberCb.h"
#include <vector>

class MusicReader {

private:
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;

	IBaseFilter *pGrabberF;
    ISampleGrabber *pGrabber;
	IBaseFilter *pNullF;
	BYTE* pBuffer;
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
	BYTE* setupSampleGrabber();
	long getSampleSize();
	BYTE* getSample();
	bool isGrabbed();
	bool isReady();
	HRESULT IsPinConnected(IPin *pPin, BOOL *pResult);
	HRESULT IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult);
	HRESULT MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult);
	HRESULT FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn);
	HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
};

#endif