/*
 *  PHMainEvents.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#ifndef PHMAINEVENTS_H
#define PHMAINEVENTS_H

#include "PHMain.h"

class PHView;
class PHViewController;

class PHMainEvents : public PHObject
{
private:
	PHView * view;
	PHViewController * viewController;
	double _screenWidth;
	double _screenHeight;
	int fps;
	bool suspended;
	bool indTiming;
public:
	double screenWidth() { return _screenWidth; };
	double screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHMakeRect(0, 0, _screenWidth, _screenHeight); };
	int framesPerSecond() { return fps; }
	static PHMainEvents * sharedInstance();
	void renderFrame(double timeElapsed);
	void appSuspended();
	void appResumed();
	void appQuits();
	void memoryWarning();
	void init(double screenX, double screenY,int FPS);
	PHView * mainView() { return view; };
	void remove(void * ud);
	
	bool independentTiming() { return indTiming; };
	void setIndependentTiming(bool i) { indTiming = i; };
};

#endif