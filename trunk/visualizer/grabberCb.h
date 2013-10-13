/* grabberCb.h
Implementations of callback methods for the sample grabber; code modified from sample at http://www.wcode.net/plus/view.php?aid=835661 to implement BufferCB */

#include <streams.h>   
#include <DShow.h>
#include <vector>
#include "qedit.h"
//   Link   to   Strmbase.lib  
using namespace std;

class   CGrabCB:   public   CUnknown,   public   ISampleGrabberCB {  
private:
	// buffer for storing byte values from the audio sample
	vector<BYTE>* drawValue;
	// length of the buffer
	long currentLength;
	// set to true when the buffer callback has finished updating the buffer
	bool ready;

public:   
	DECLARE_IUNKNOWN;   
    
	STDMETHODIMP   NonDelegatingQueryInterface(REFIID   riid,   void   **ppv);    
	STDMETHODIMP   SampleCB(double   SampleTime,   IMediaSample   *pSample);   
	STDMETHODIMP   BufferCB(double   SampleTime,   BYTE   *pBuffer,   long   BufferLen);
	void CGrabCB::setDrawValue(vector<BYTE>* value_);
	bool isReady();
	long getLength();
	//   Constructor   
	CGrabCB(   );   
};   