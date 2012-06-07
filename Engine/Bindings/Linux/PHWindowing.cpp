/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHMain.h"
#include "PHWindowing.h"
#include "PHX11.h"
#include "PHGameManager.h"
#include "PHEventHandler.h"
#include <GL/glx.h>

static bool PHNamesInitialized = false;
static string PHFullExecutablePath;
static string PHResourcePath;
static string PHAppName;
static int PHWFlags;
static PHGameManager * PHWGameManager;
static double PHWLastTime;

void PHWInitNames()
{
    if (!PHNamesInitialized)
    {
        #define BUFFERSIZE 1024
        char buf[BUFFERSIZE];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
 
        if (len != -1) 
        {
            buf[len] = '\0';
            PHFullExecutablePath = buf;
            ssize_t t = len - 1;
            while (t && buf[t-1] != '/')
                t--;
            PHAppName = buf+t;
            PHResourcePath = PHFullExecutablePath + "-rsrc";
            PHLog("Resource name: \"%s\"", PHResourcePath.c_str());
        }
        PHNamesInitialized = true;
    }
}

void * PHWCreateWindow(const string & title, const PHWVideoMode & vm, int flags, void (*entryPoint)(PHGameManager *), void * ud);

int PHWMain(int argc, char * argv[], const PHWVideoMode & vm, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHWFlags = flags;
    PHWInitNames();
    PHWCreateWindow(PHAppName, vm, flags, entryPoint, ud);
    PHWLastTime = PHTime::getTime();
    PHXMainLoop();
    PHXDestroyContext();
    return 0;
}

void PHWRender()
{
    PHGameManager * gm = PHWGameManager;
    int flags = PHWFlags;

    double frameInterval = 1.0f/gm->framesPerSecond();
    double elapsedTime;
    double time;
    if (flags & PHWFrameAnimation)
        elapsedTime = frameInterval;
    else 
    {
        time = PHTime::getTime();
        elapsedTime = (time - PHWLastTime);
        if (flags & PHWVSync)
            elapsedTime = round(elapsedTime/frameInterval)*frameInterval;
    }
    PHGameManager::globalFrame(elapsedTime);
    gm->processInput();
    gm->renderFrame(elapsedTime);
    PHWLastTime = time;
}

#define PHWMaxButtons 10
static bool PHWMouseMask[PHWMaxButtons];

static PHWVideoMode PHWCurrentVideoMode;

void * PHWCreateWindow(const string & title, const PHWVideoMode & vm, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    int resolutionX = vm.width, resolutionY = vm.height;
    int rr = vm.refresh;
    PHXCreateContext(title, resolutionX, resolutionY, rr, int(vm.type), flags);
    PHWCurrentVideoMode = vm;
    PHWCurrentVideoMode.width = resolutionX;
    PHWCurrentVideoMode.height = resolutionY;
    
    PHGameManager * gm = PHWGameManager = new PHGameManager;
    gm->setUserData(ud);
    PHGameManagerInitParameters init;
    init.screenWidth = resolutionX;
    init.screenHeight = resolutionY;
    init.fps = rr;
    init.resourcePath = PHResourcePath;
    init.entryPoint = entryPoint;
    if (flags & PHWRemote)
        gm->setUsesRemote(true);
    if (flags & PHWShowFPS)
        gm->setShowsFPS(true);
    if (flags & PHWVSync)
        gm->setFpsCapped(true);
    
    gm->init(init);
    
    memset(PHWMouseMask, 0, sizeof(PHWMouseMask));

    PHWSetVSync((flags & PHWVSync)!=0);
}


void PHWMouseDown(int button, int x, int y)
{
    PHWMouseMask[button] = true;
    y = PHWGameManager->screenHeight() - y;
    PHWGameManager->eventHandler()->touchDown(PHPoint(x, y), (void*)(size_t)(button));
}

void PHWMouseUp(int button, int x, int y)
{
    PHWMouseMask[button] = false;
    y = PHWGameManager->screenHeight() - y;
    PHWGameManager->eventHandler()->touchUp(PHPoint(x, y), (void*)(size_t)(button));
}

void PHWMouseMoved(int x, int y)
{
    y = PHWGameManager->screenHeight() - y;
    for (int i = 0; i<PHWMaxButtons; i++)
        if (PHWMouseMask[i])
            PHWGameManager->eventHandler()->touchMoved(PHPoint(x, y), (void*)(size_t)(i));
}

static bool PHHasVSync = false;
void PHWSetVSync(bool vsync)
{
    if (PHHasVSync == vsync) return;
    PHHasVSync = vsync;
    PHXSetVSync(vsync);
    PHWFlags = (PHWFlags & ~PHWVSync) | (vsync?PHWVSync:0);
}

void PHWReconfigure(int w, int h)
{
    PHGameManager * gm = PHWGameManager;
    if ((w != int(gm->screenWidth())) || (h != int(gm->screenHeight())))
        gm->setScreenSize(w, h);
}

void PHWSetVideoMode(const PHWVideoMode & vm)
{
    int w = vm.width, h = vm.height, r = vm.refresh;
    PHXSetVideoMode(w, h, r, (int)vm.type, PHWFlags & PHWResizable);
    PHWGameManager->setFramesPerSecond(r);
    PHWCurrentVideoMode = vm;
}

const PHWVideoMode & PHWGetVideoMode()
{
    PHWCurrentVideoMode.width = PHWGameManager->screenWidth();
    PHWCurrentVideoMode.height = PHWGameManager->screenHeight();
    return PHWCurrentVideoMode;
}

bool PHWGetVSync()
{
    return (PHWFlags & PHWVSync)!=0;
}

void PHWSetResizable(bool r)
{
    PHWFlags = (PHWFlags & ~PHWResizable) | (r?PHWResizable:0);
    PHWSetVideoMode(PHWGetVideoMode());
}

bool PHWGetResizable()
{
    return (PHWFlags & PHWResizable)!=0;
}

const vector<PHWVideoMode> & PHWSupportedModes()
{
    return PHXVideoModes();
}


void PHWSetTitle(const string & title)
{
    PHXSetTitle(title);
}

void PHWClose()
{
    PHXTerminate();
}

