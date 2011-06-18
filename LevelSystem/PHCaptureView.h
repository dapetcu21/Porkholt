/*
 *  PHCaptureView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHCAPTUREVIEW_H
#define PHCAPTUREVIEW_H

#include "PHMain.h"

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
	PHCaptureView() : PHView(), l(NULL), mutex(NULL), sm1(NULL), sm2(NULL) {};
	PHCaptureView(const PHRect &frame) : PHView(frame), l(NULL), mutex(NULL), sm1(NULL), sm2(NULL) {};
	void setQueue(list<PHPoint> * q) { l = q; }
	void setMutex(PHMutex * m) { mutex = m; }
	void setSemaphores(PHSemaphore * s1, PHSemaphore * s2) { sm1 = s1; sm2= s2; }
	void setPaused(bool * p) { paused = p; lastPaused=*p; }
protected:
	virtual void render();
	virtual void touchEvent(PHTouch * touch);
};

#endif
