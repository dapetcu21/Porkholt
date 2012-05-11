//
//  PHStartGame.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/7/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHMain.h"
#include "PHStartGame.h"
#include "PHGameManager.h"
#include "PHEventHandler.h"
#include <GL/glx.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

bool PHNamesInitialized = false;
string PHFullExecutablePath;
string PHResourcePath;
string PHAppName;

void PHInitNames()
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

int PHStartGame(int argc, char * argv[], unsigned int resX, unsigned int resY, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    glutInit(&argc, argv);
 
    PHInitNames();
 
    void * window = PHCreateWindow(PHAppName, resX, resY, flags, entryPoint, ud);
    
    glutMainLoop();
    return 0;
}

class PHWindow
{
public:
    int wid;
    PHGameManager * gm;
    double time, lastTime;
    int flags;
    bool full;
    int but[4];
    static map<int, PHWindow* > wnds;
    PHWindow(int w, PHGameManager * g) : wid(w), gm(g) {
        wnds.insert(make_pair<int, PHWindow*>(w,this));
        but[0] = but[1] = but[2] = but[3] = 0;
    }
    static PHWindow * windowForID(int wid) { return wnds[wid]; }
    ~PHWindow() { wnds.erase(wid); }
};

map<int, PHWindow* > PHWindow::wnds;

typedef GLvoid (*glXSwapIntervalFunc) (GLint);
typedef GLvoid (*glXWaitVideoSyncFunc) (int, int, int *);
glXSwapIntervalFunc glXSwapInterval = 0;
glXWaitVideoSyncFunc glXWaitVideoSync = 0;

void PHGLUTRender()
{
    PHWindow * w = PHWindow::windowForID(glutGetWindow());
    PHGameManager * gm = w->gm;
    int flags = w->flags;
    
    double frameInterval = 1.0f/gm->framesPerSecond();
    double elapsedTime;
    if (flags & PHStartGame_frameAnimation)
        elapsedTime= frameInterval;
    else 
    {
        w->lastTime = w->time;
        w->time = PHTime::getTime();
        elapsedTime = (w->time - w->lastTime);
        if (flags & PHStartGame_VSync)
            elapsedTime = round(elapsedTime/frameInterval)*frameInterval;
    }
    PHGameManager::globalFrame(elapsedTime);
    gm->setFrameBegin(w->time);
    gm->processInput();
    gm->renderFrame(elapsedTime);
    
    glFlush();
    if (glXWaitVideoSync)
    {
        int count;
        glXWaitVideoSync(1, 0, &count);
        //static int lastF;
        //if (count!=lastF+1) PHLog("count: %d", count);
        //lastF = count;
    }
    glutSwapBuffers();
}

void PHGLUTMouse(int button, int state, int x, int y)
{
    PHWindow * w = PHWindow::windowForID(glutGetWindow());
    PHGameManager * gm = w->gm;
    y = gm->screenHeight() - y;

    size_t b;
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            b = 1;
            break;
        case GLUT_RIGHT_BUTTON:
            b = 2;
            break;
        case GLUT_MIDDLE_BUTTON:
            b = 3;
            break;
        default:
            b = 0;
    }
    if (!b) return;
    
    if (state == GLUT_UP)
    {
        gm->eventHandler()->touchUp(PHPoint(x, y), (void*)b);
        w->but[b] = 0;
    }
    
    if (state == GLUT_DOWN)
    {
        gm->eventHandler()->touchDown(PHPoint(x, y), (void*)b);
        w->but[b] = 1;
    }
}

void PHGLUTMotion(int x, int y)
{
    PHWindow * w = PHWindow::windowForID(glutGetWindow());
    PHGameManager * gm = w->gm;
    y = gm->screenHeight() - y;
    
    for (size_t b = 1; b<=3; b++)
        if (w->but[b])
            gm->eventHandler()->touchMoved(PHPoint(x, y), (void*)b);
}

void PHGLUTReshape(int x, int y)
{
    PHWindow * w = PHWindow::windowForID(glutGetWindow());
    PHGameManager * gm = w->gm;
    gm->setScreenSize(x, y);
}

void * PHCreateWindow(const string & title, unsigned int resolutionX, unsigned int resolutionY, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHInitNames();
    
    glutInitWindowSize(resolutionX, resolutionY);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    int wid = glutCreateWindow(title.c_str());
    
    PHGameManager * gm = new PHGameManager;
    gm->setUserData(ud);
    PHGameManagerInitParameters init;
    init.screenWidth = resolutionX;
    init.screenHeight = resolutionY;
    init.fps = 60;
    init.resourcePath = PHResourcePath;
    init.entryPoint = entryPoint;
    if (flags & PHStartGame_Remote)
        gm->setUsesRemote(true);
    if (flags & PHStartGame_ShowFPS)
        gm->setShowsFPS(true);
    if (flags & PHStartGame_VSync)
        gm->setFpsCapped(true);
    
    PHWindow * win = new PHWindow(wid, gm);
    win->time = PHTime::getTime();
    win->flags = flags;
    win->full = false;
        
    gm->init(init);
    
    PHWindowSetVSync(win, (flags & PHStartGame_VSync)!=0);
    
    glutDisplayFunc(PHGLUTRender);
    glutIdleFunc(PHGLUTRender);
    glutMouseFunc(PHGLUTMouse);
    glutMotionFunc(PHGLUTMotion);
    glutReshapeFunc(PHGLUTReshape);
    
    if (flags & PHStartGame_FullScreen)
        PHWindowSetFullScreen(win, true);
    
    return win;
}

typedef glXSwapIntervalFunc (*getProcAddressFunc) (const char *);

void PHWindowSetVSync(void * window, bool vsync)
{
    PHWindow * w = (PHWindow*)window; 
    glutSetWindow(w->wid);
        
    GLint iInterval = (vsync ? 1 : 0);

    getProcAddressFunc getProcAddress = (getProcAddressFunc)glXGetProcAddressARB;
    
    Display * display = glXGetCurrentDisplay();
    set<string> extensions;
    const char * ext = (char*) glXQueryExtensionsString(display, DefaultScreen( display ));
    while (ext && ext[0])
    {
        const char * pos = strpbrk(ext, " ");
        if (!pos)
            pos = ext+strlen(ext);
            extensions.insert(string(ext,pos-ext));
            if (!pos[0])
                ext = pos;
            else
                ext = pos+1;
    }
    
    if (extensions.count("GLX_SGI_swap_control")) {
        glXSwapInterval = (glXSwapIntervalFunc) getProcAddress("glXSwapIntervalSGI");
        glXWaitVideoSync = (glXWaitVideoSyncFunc) getProcAddress("glXWaitVideoSyncSGI");
    } else if (extensions.count("GLX_EXT_swap_control")) {
        glXSwapInterval = (glXSwapIntervalFunc) getProcAddress("glXSwapIntervalEXT");
        glXWaitVideoSync = (glXWaitVideoSyncFunc) getProcAddress("glXWaitVideoSyncEXT");
    } else if (extensions.count("GLX_MESA_swap_control")) {
        glXSwapInterval = (glXSwapIntervalFunc) getProcAddress("glXSwapIntervalMESA");
        glXWaitVideoSync = (glXWaitVideoSyncFunc) getProcAddress("glXWaitVideoSyncMESA");
    } else if (extensions.count("GLX_OML_sync_control")) {
        glXSwapInterval = (glXSwapIntervalFunc) getProcAddress("glXSwapIntervalOML");
        glXWaitVideoSync = (glXWaitVideoSyncFunc) getProcAddress("glXWaitVideoSyncOML");
    } 
    else
    {
        PHLog("Unable to switch sync-to-vblank on.\n");
        return;
    }
      
    glXSwapInterval(iInterval);
}


void PHWindowSetFullScreen(void * window, bool fullScreen)
{
    PHWindow * win = (PHWindow*)window;
    if (win->full == fullScreen) return;
    win->full = fullScreen;
    glutSetWindow(win->wid);
    glutFullScreenToggle();
}

void PHWindowSetTitle(void * window, const string & title)
{
}

void PHCloseWindow(void * window)
{
}
