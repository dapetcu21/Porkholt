/*
 *  PHMainEvents.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHMAINEVENTS_H
#define PHMAINEVENTS_H

#include "PHMain.h"

class PHView;
class PHViewController;
class PHNavigationController;
class PHRemote;

class PHMainEvents : public PHObject
{
private:
	PHView * view;
	PHNavigationController * viewController;
	double _screenWidth;
	double _screenHeight;
	int fps;
	bool suspended;
    bool loaded;
    
#ifdef PH_SIMULATOR
    PHRemote * remote;
#endif
    
    void entryPoint();
    
    void setProjection();
    void setScreenSize(double w, double h);
    
    void * ud;
public:
    PHMainEvents();
	double screenWidth() { return _screenWidth; };
	double screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
	int framesPerSecond() { return fps; }
	static PHMainEvents * sharedInstance();
	void renderFrame(double timeElapsed);
	void appSuspended();
	void appResumed();
    void _appResumed(PHObject * sender, void * ud);
    void _appSuspended(PHObject * sender, void * ud);
	void appQuits();
	void memoryWarning();
	void init(double screenX, double screenY,int FPS);
	PHView * mainView() { return view; };
	void remove(void * ud);
    
    void * userData() { return ud; }
    void setUserData(void * u) { ud = u; }
    
    void processInput();

    enum interfaceType
    {
        interfaceSD = 0,
        interfaceHD,
        numInterfaceTypes
    };
    int interfaceType();
};

#endif