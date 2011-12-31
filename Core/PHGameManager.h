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

class PHGameManagerInitParameters
{
public:
    PHGameManagerInitParameters() : screenWidth(480), screenHeight(340), fps(60), dpi(150) {}

    double screenWidth;
    double screenHeight;
    int fps;
    double dpi;
};

class PHGameManager : public PHObject
{
private:
	PHView * view;
	PHNavigationController * viewController;
    PHEventHandler * evtHandler;
	double _screenWidth;
	double _screenHeight;
    double dpi;
	int fps;
	bool suspended;
    bool loaded;
    static int globalHD;
    bool hd;
    
#ifdef PH_SIMULATOR
    PHRemote * remote;
#endif
    
    void entryPoint();
    
    void setProjection();
    void * ud;
    
    void updateHD();
    
public:
    PHGameManager();
    ~PHGameManager();
    void init(const PHGameManagerInitParameters & params);
	double screenWidth() { return _screenWidth; };
	double screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
    void setScreenSize(double w, double h);
	int framesPerSecond() { return fps; }
    double dotsPerInch() { return dpi; }
	void renderFrame(double timeElapsed);
	void appSuspended();
	void appResumed();
    bool isHD() { return hd; }
    static bool isGloballyHD() { return globalHD > 0; }
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
    PHView * rootView() { return view; }
};

#endif