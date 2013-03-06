/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHWindowing.h>
#include "PHX11.h"
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/IO/PHLinkDirectory.h>
#include <GL/glx.h>

static bool PHNamesInitialized = false;
static string PHFullExecutablePath;
static string PHResourcePath;
static string PHAppName;
static int PHWFlags;
static PHGameManager * PHWGameManager;
static int PHWArgc;
static char ** PHWArgv;

void PHWInitNames()
{
    if (!PHNamesInitialized)
    {
        #define BUFFERSIZE 1024
        char buf[BUFFERSIZE];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
 
        if ((len == -1) && (PHWArgc>0))
        {
            len = strlen(PHWArgv[0]);
            if (len + 6 <= BUFFERSIZE)
                memcpy(buf, PHWArgv[0], len);
            else
                len = -1;
        }
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
    PHWArgc = argc;
    PHWArgv = argv;
    PHWInitNames();
    PHWCreateWindow(PHAppName, vm, flags, entryPoint, ud);
    PHXMainLoop();
    PHXDestroyContext();
    return 0;
}

void PHWRender()
{
    PHGameManager * gm = PHWGameManager;
    gm->processInput();
    gm->renderFrame();
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
    
    PHAutoreleasePool ap;
    PHGameManager * gm = PHWGameManager = new PHGameManager;
    gm->setUserData(ud);
    PHGameManagerInitParameters init;
    init.screenWidth = resolutionX;
    init.screenHeight = resolutionY;
    init.fps = rr;
    try { 
        PHDirectory * f = new PHLinkDirectory(PHInode::directoryAtFSPath(PHResourcePath.c_str()));
        f->autorelease();
        init.setResourceDirectory(f); }
    catch (const string & ex) {
        PHLog("Can't load resource directory: %s", ex.c_str());
    }
    init.entryPoint = entryPoint;
    PHWApplyFlags(gm, flags);
    
    gm->init(init);
    
    memset(PHWMouseMask, 0, sizeof(PHWMouseMask));

    PHWSetVSync((flags & PHWVSync)!=0);
}

static PHPoint adjustPoint(int x, int y)
{
    return PHPoint(
        x *  2.0f / PHWGameManager->screenWidth() - 1,
        y * -2.0f / PHWGameManager->screenHeight() + 1
        );
}

void PHWMouseDown(int button, int x, int y)
{
    PHWMouseMask[button] = true;
    PHWGameManager->eventHandler()->touchDown(adjustPoint(x, y), (void*)(size_t)(button));
}

void PHWMouseUp(int button, int x, int y)
{
    PHWMouseMask[button] = false;
    PHWGameManager->eventHandler()->touchUp(adjustPoint(x, y), (void*)(size_t)(button));
}

void PHWMouseMoved(int x, int y)
{
    PHPoint p = adjustPoint(x, y);
    for (int i = 0; i<PHWMaxButtons; i++)
        if (PHWMouseMask[i])
            PHWGameManager->eventHandler()->touchMoved(p, (void*)(size_t)(i));
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

PHWVideoMode PHWGetVideoMode()
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

