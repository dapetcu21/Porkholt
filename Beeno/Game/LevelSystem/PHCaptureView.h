/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCAPTUREVIEW_H
#define PHCAPTUREVIEW_H

#include <Porkholt/Core/PHView.h>

class PHMutex;
class PHSemaphore;

class PHCaptureView : public PHView
{
private:
	list<PHPoint> * l;
	PHMutex * mutex;
	PHSemaphore * sm1, * sm2;
	bool * paused;
	bool lastPaused;
public:
#define PHCAPTUREVIEW_INIT l(NULL), mutex(NULL), sm1(NULL), sm2(NULL), lastPaused(false)
	PHCaptureView() : PHView(), PHCAPTUREVIEW_INIT{};
	PHCaptureView(const PHRect &frame) : PHView(frame), PHCAPTUREVIEW_INIT {};
    virtual ~PHCaptureView();
    
	void setQueue(list<PHPoint> * q) { l = q; }
	void setMutex(PHMutex * m);
    PHMutex * getMutex() { return mutex; }
	void setSemaphores(PHSemaphore * s1, PHSemaphore * s2) { sm1 = s1; sm2= s2; }
	void setPaused(bool * p) { paused = p; lastPaused=(*p); }
protected:
	virtual void render();
	virtual void touchEvent(PHEvent * touch);
};

#endif
