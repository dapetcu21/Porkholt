/*
 *  PHGameManager.h
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHGAMEMANAGER_H
#define PHGAMEMANAGER_H

#include "PHMain.h"

class PHView;
class PHViewController;
class PHNavigationController;
class PHRemote;
class PHEventHandler;

class PHGameManager : public PHObject
{
private:
	PHView * view;
	PHNavigationController * viewController;
    PHEventHandler * evtHandler;
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
    void * ud;
    
public:
    PHGameManager();
    void init(double screenX, double screenY,int FPS);
	double screenWidth() { return _screenWidth; };
	double screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
    void setScreenSize(double w, double h);
	int framesPerSecond() { return fps; }
	void renderFrame(double timeElapsed);
	void appSuspended();
	void appResumed();
    void _appResumed(PHObject * sender, void * ud);
    void _appSuspended(PHObject * sender, void * ud);
	void appQuits();
	void memoryWarning();
	PHView * mainView() { return view; };
	void remove(void * ud);
    
    void * userData() { return ud; }
    void setUserData(void * u) { ud = u; }
    
    void processInput();
    PHEventHandler * eventHandler() { return evtHandler; }

    enum interfaceType
    {
        interfaceSD = 0,
        interfaceHD,
        numInterfaceTypes
    };
    static int interfaceType();
    
    PHNavigationController * navigationController() { return viewController; }
};

#endif