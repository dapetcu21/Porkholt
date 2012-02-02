/*
 *  PHGameManager.cpp
 *  Porkholt
 *
 *  Created by Marius Petcu on 12/15/10.
 *  Copyright 2010 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHGameManager.h"
#ifdef PH_SIMULATOR
#include "PHRemote.h"
#endif
#include "PHView.h"
#include "PHFileManager.h"
#include "PHImageAnimator.h"
#include "PHAnimatorPool.h"
#include "PHNavigationController.h"
#include "PHEventHandler.h"
#include "PHSoundManager.h"

PHGameManager::PHGameManager() : view(NULL), viewController(NULL), loaded(false), openGLStates(0)
{}

PHGameManager::~PHGameManager()
{
    if (evtHandler)
        evtHandler->release();
    if (sndManager)
        sndManager->release();
    if (hd)
        --globalHD;
}

int PHGameManager::globalHD = 0;

void PHGameManager::updateHD()
{
    ph_float x = _screenWidth;
    ph_float y = _screenHeight;
    ph_float diagsq = x*x+y*y;
    bool newhd = (diagsq > 500000);
    if (newhd && !hd)
        ++globalHD;
    if (!newhd && hd)
        --globalHD;
    hd = newhd;
}

void PHGameManager::init(const PHGameManagerInitParameters & params)
{
	fps = params.fps;
	_screenWidth = params.screenWidth;
	_screenHeight = params.screenHeight;
    resPath = params.resourcePath;
    dpi = params.dpi;
	suspended = 0;
    loaded = true;
	setUserData(ud);
    
    hd = false;
    updateHD();
    
	PHThread::mainThread();

#ifdef PH_SIMULATOR
    remote = new PHRemote(this);
    remote->start();
#endif
	
    evtHandler = new PHEventHandler(this);
#ifdef PH_IPHONE_OS
    sndManager = new PHSoundManager(resPath + "/snd/fx");
#endif
    
    setProjection();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    PHGLHasCapability(PHGLCapabilityOpenGLES);
    
    entryPoint();
    view->setGameManager(this);
}

void PHGameManager::setModelViewMatrix(const PHMatrix & m)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m.floats());
    _modelView = m;
}

void PHGameManager::setProjectionMatrix(const PHMatrix & m)
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(m.floats());
    _projection = m;
}

void PHGameManager::setProjection()
{
    glViewport(0, 0, _screenWidth, _screenHeight);
    PHMatrix m;
    m.setToTranslation(PHPoint(-1,-1));
    m.scale(PHSize(2.0f/(_screenWidth), 2.0f/(_screenHeight)));
    setProjectionMatrix(m);
}

void PHGameManager::setScreenSize(ph_float w, ph_float h)
{
    _screenWidth = w;
    _screenHeight = h;
    updateHD();
    setProjection(); 
    if (view)
        view->setFrame(PHRect(0,0,_screenWidth,_screenHeight));
}

void PHGameManager::processInput()
{
#ifdef PH_SIMULATOR
    remote->processPendingPackets();
#endif
}

void PHGameManager::globalFrame(ph_float timeElapsed)
{
    PHAnimatorPool::mainAnimatorPool()->advanceAnimation(timeElapsed);
    PHThread::mainThread()->processQueue();
}

void PHGameManager::renderFrame(ph_float timeElapsed)
{	
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
		
    setModelViewMatrix(PHIdentityMatrix);
    
	if (viewController)  
		viewController->_updateScene(timeElapsed);
    
	view->render();
}

void PHGameManager::setOpenGLStates(uint32_t states)
{
    if (states & PHGLResetAllStates)
    {
        states = 0;
        openGLStates = 0xffffffff;
    }
    
    int xr = states^openGLStates;
    openGLStates = states;
    
    if (xr & PHGLVertexArray)
    {
        if (states & PHGLVertexArray)
            glEnableClientState(GL_VERTEX_ARRAY);
        else
            glDisableClientState(GL_VERTEX_ARRAY);
    }
    
    
    if (xr & PHGLColorArray)
    {
        if (states & PHGLColorArray)
            glEnableClientState(GL_COLOR_ARRAY);
        else
            glDisableClientState(GL_COLOR_ARRAY);
    }
    
    if (xr & PHGLTextureCoordArray)
    {
        if (states & PHGLTextureCoordArray)
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        else
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    
    if (xr & PHGLTexture)
    {
        if (states & PHGLTexture)
            glEnable(GL_TEXTURE_2D);
        else
            glDisable(GL_TEXTURE_2D);
    }
}

void PHGameManager::_appSuspended(PHObject * sender, void * ud)
{
	if (viewController)
	{
		viewController->_viewWillDisappear();
		viewController->_viewDidDisappear();
	}
}

void PHGameManager::_appResumed(PHObject * sender, void * ud)
{
    if (viewController)
	{
		viewController->_viewWillAppear();
		viewController->_viewDidAppear();
	}
}

void PHGameManager::appSuspended()
{
    if (!loaded) return;
    if (!loaded) return;
	if (suspended) return;
	suspended = true;
	PHMessage::messageWithName("appSuspended")->broadcast(this, NULL);
    PHLog("appSuspended");
#ifdef PH_SIMULATOR
    remote->stop();
#endif
    PHThread::mainThread()->executeOnThread(PHInv(this,PHGameManager::_appSuspended, NULL), false);
}

void PHGameManager::appResumed()
{
    if (!loaded) return;
    if (!suspended) return;
	suspended = false;
	PHLog("appResumed");
	PHMessage::messageWithName("appResumed")->broadcast(this, NULL);
#ifdef PH_SIMULATOR
    try {
        remote->start();
    } catch (string err) {
        PHLog("URemote: %s",err.c_str());
    }
#endif
    PHThread::mainThread()->executeOnThread(PHInv(this,PHGameManager::_appResumed, NULL), false);
}


void PHGameManager::appQuits()
{
    if (!loaded) return;
	//This isn't guaranteed to be called
	//Save all stuff in PHGameManager::appSuspended()
	PHLog("appQuits");
#ifdef PH_SIMULATOR
    delete remote;
#endif
}

void PHGameManager::memoryWarning()
{
	PHLog("memoryWarning");
}

int PHGameManager::interfaceType()
{
    if (isGloballyHD())        
        return interfaceHD;
    return interfaceSD;
}

const string PHGameManager::imageDirectory()
{
    return resourcePath() + "/img/";
}

const string PHGameManager::fontDirectory()
{
    return resourcePath() + "/fnt/";
}

const string PHGameManager::musicNamed(const string & name)
{
    return resourcePath() + "/snd/music/"+name+".mp3";
}