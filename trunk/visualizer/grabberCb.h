// Implementations of callback methods for the sample grabber; code from sample at http://www.wcode.net/plus/view.php?aid=835661

#include <streams.h>   
#include <DShow.h>
#include <vector>
#include "qedit.h"
  //   Link   to   Strmbase.lib  
using namespace std;

  class   CGrabCB:   public   CUnknown,   public   ISampleGrabberCB   
  {   
  private:
	  BYTE currentBuffer;
	  long currentLength;
	  vector<BYTE>* drawValue;
	  bool ready;

  public:   
          DECLARE_IUNKNOWN;   
    
          STDMETHODIMP   NonDelegatingQueryInterface(REFIID   riid,   void   **ppv)   
          {   
                  if(   riid   ==   IID_ISampleGrabberCB   )   
                  {   
                          return   GetInterface((ISampleGrabberCB*)this,   ppv);   
                  }   
                  return   CUnknown::NonDelegatingQueryInterface(riid,   ppv);   
          }   
    
          //   ISampleGrabberCB   methods   
          STDMETHODIMP   SampleCB(double   SampleTime,   IMediaSample   *pSample)   
          {   
                  //printf("Sample   time:   %f\n",   SampleTime);   
                  return   S_OK;   
          }   
    
          STDMETHODIMP   BufferCB(double   SampleTime,   BYTE   *pBuffer,   long   BufferLen)   
          {  
				  ready = false;
				  // line mode
				  //(*drawValue).push_back(pBuffer[0]);
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

		  BYTE getBuffer() {
			  int a = 0;
			  return currentBuffer;
		  }

		  void setBuffer(BYTE bufferValue) {
			  currentBuffer = bufferValue;
		  }
		  
		  bool isReady() {
			  return ready;
		  }

		  long getLength() {
			  return currentLength;
		  }

		  void setDrawValue(vector<BYTE>* value_) {
			  drawValue = value_;
		  }
            
          //   Constructor   
          CGrabCB(   )   :   CUnknown("SGCB",   NULL)   
          { ready = false;  }   
  };   