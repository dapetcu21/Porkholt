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
#include "PHImageInitPool.h"
#include "PHFontInitPool.h"

class PHView;
class PHViewController;
class PHNavigationController;
class PHRemote;
class PHEventHandler;
class PHSoundManager;

class PHGameManagerInitParameters
{
public:
    PHGameManagerInitParameters() : screenWidth(480), screenHeight(340), fps(60), dpi(150), resourcePath("./rsrc") {}

    ph_float screenWidth;
    ph_float screenHeight;
    int fps;
    ph_float dpi;
    string resourcePath;
};

enum PHGLStates
{
    PHGLVertexArray = 1<<0,
    PHGLColorArray = 1<<1,
    PHGLTextureCoordArray = 1<<2,
    PHGLTexture = 1<<3,
    PHGLResetAllStates = 1<<4
};

class PHGameManager : public PHObject, public PHImageInitPool, public PHFontInitPool
{
private:
	PHView * view;
	PHNavigationController * viewController;
    PHEventHandler * evtHandler;
    PHSoundManager * sndManager;
	ph_float _screenWidth;
	ph_float _screenHeight;
    ph_float dpi;
	int fps;
	bool suspended;
    bool loaded;
    static int globalHD;
    bool hd;
    string resPath;
    
#ifdef PH_SIMULATOR
    PHRemote * remote;
#endif
    
    void entryPoint();
    
    void setProjection();
    void * ud;
    
    void updateHD();
    
    uint32_t openGLStates;
    PHMatrix _modelView,_projection;
    
public:
    PHGameManager();
    ~PHGameManager();
    void init(const PHGameManagerInitParameters & params);
	ph_float screenWidth() { return _screenWidth; };
	ph_float screenHeight() { return _screenHeight; };
	PHRect screenBounds() { return PHRect(0, 0, _screenWidth, _screenHeight); };
    void setScreenSize(ph_float w, ph_float h);
	int framesPerSecond() { return fps; }
    ph_float dotsPerInch() { return dpi; }
    const string & resourcePath() { return resPath; }
    void setResourcePath(const string & r) { resPath = r; }
	void renderFrame(ph_float timeElapsed);
    static void globalFrame(ph_float timeElapsed);
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
    const string imageDirectory();
    const string fontDirectory();
    const string musicNamed(const string & name);
    
    void * userData() { return ud; }
    void setUserData(void * u) { ud = u; }
    
    void processInput();
    PHEventHandler * eventHandler() { return evtHandler; }
    PHSoundManager * soundManager() { return sndManager; }
    
    enum interfaceType
    {
        interfaceSD = 0,
        interfaceHD,
        numInterfaceTypes
    };
    static int interfaceType();
    
    PHNavigationController * navigationController() { return viewController; }
    PHView * rootView() { return view; }
    
    void setOpenGLStates(uint32_t states);
    void setModelViewMatrix(const PHMatrix & m);
    PHMatrix modelViewMatrix() { return _modelView; }
    void setProjectionMatrix(const PHMatrix & m);
    PHMatrix projectionMatrix() { return _projection; }
};

#endif