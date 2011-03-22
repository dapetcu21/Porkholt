/*
 *  PHCaptureView.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHCAPTUREVIEW_H
#define PHCAPTUREVIEW_H

#include "PHMain.h"

class PHMutex;

class PHCaptureView : public PHView
{
private:
	list<PHPoint> * l;
	PHMutex * mutex;
public:
	PHCaptureView() : PHView(), l(NULL), mutex(NULL) {};
	PHCaptureView(const PHRect &frame) : PHView(frame), l(NULL), mutex(NULL) {};
	void setQueue(list<PHPoint> * q) { l = q; }
	void setMutex(PHMutex * m) { mutex = m; }
protected:
	virtual void touchEvent(PHTouch * touch);
};

#endif
