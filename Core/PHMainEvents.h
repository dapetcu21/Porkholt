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

class PHMainEvents : public PHObject
{
private:
	PHView * view;
	double _screenWidth;
	double _screenHeight;
	bool suspended;
public:
	double screenWidth() { return _screenWidth; };
	double screenHeight() { return _screenHeight; };
	static PHMainEvents * sharedInstance();
	void renderFrame(double timeElapsed);
	void appSuspended();
	void appResumed();
	void appQuits();
	void memoryWarning();
	void init(double screenX, double screenY);
	PHView * mainView() { return view; };
	void remove(void * ud);
};

#endif