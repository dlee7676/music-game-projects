#include "grabberCb.h"  
    
CGrabCB::CGrabCB(   )   :   CUnknown("SGCB",   NULL)   
{ ready = false;  }  

STDMETHODIMP CGrabCB::NonDelegatingQueryInterface(REFIID   riid,   void   **ppv)   
{   
        if(   riid   ==   IID_ISampleGrabberCB   )   
        {   
                return   GetInterface((ISampleGrabberCB*)this,   ppv);   
        }   
        return   CUnknown::NonDelegatingQueryInterface(riid,   ppv);   
} 

//   ISampleGrabberCB   methods   
//   SampleCB is not currently being used
STDMETHODIMP   CGrabCB::SampleCB(double   SampleTime,   IMediaSample   *pSample)   
{   
        //printf("Sample   time:   %f\n",   SampleTime);   
        return   S_OK;   
}   
//   Stores the current sample buffer in a BYTE vector, which will be used when rendering
STDMETHODIMP   CGrabCB::BufferCB(double   SampleTime,   BYTE   *pBuffer,   long   BufferLen)   
{  
		ready = false;
		(*drawValue).clear();
		for (int i = 0; i < BufferLen; i++) {
		(*drawValue).push_back(pBuffer[i]);
		int a = 0;
		}
		int a = 0;
		currentLength = BufferLen;
		ready = true;
        return   S_OK;   
}  

// getters and setters
bool CGrabCB::isReady() {
	return ready;
}
long CGrabCB::getLength() {
	return currentLength;
}
void CGrabCB::setDrawValue(vector<BYTE>* value_) {
	drawValue = value_;
} 
