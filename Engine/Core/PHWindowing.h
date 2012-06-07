/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHWINDOWING_H
#define PHWINDOWING_H

#include "PHMain.h"
#include "PHGameManager.h"

enum PHStartGame_flags
{
    PHWGLES1 = 1<<0,
    PHWFrameAnimation = 1<<2,
    PHWResizable = 1<<5,
    PHWVSync = 1<<6,
    PHWRemote = 1<<7,
    PHWShowFPS = 1<<8,
    PHWDepthBuffer = 1<<3,
    PHWStencilBuffer = 1<<4
};

class PHWVideoMode
{
public:
    int width, height, refresh;
    enum windowType {
        Windowed,
        FullscreenWindowed,
        Fullscreen
    } type;

    PHWVideoMode() : width(800), height(600), refresh(60), type(Windowed) {}
    PHWVideoMode(int w, int h, int r, enum windowType t) : width(w), height(h), refresh(r), type(t) {}
    PHWVideoMode(const PHWVideoMode & o) : width(o.width), height(o.height), refresh(o.refresh), type(o.type) {}
    PHWVideoMode(enum windowType t) : width(800), height(600), refresh(60), type(t) {}
    PHWVideoMode(int w, int h, enum windowType t) : width(w), height(h), refresh(60), type(t) {}
    
    bool operator < (const PHWVideoMode & o) const
    {
        if (type != o.type)
            return type < o.type;
        if (width * height != o.width * o.height)
            return width * height > o.width * o.height;
        return refresh > o.refresh;
    }
};

int PHWMain(int argc, char * argv[], const PHWVideoMode & vmode, int flags, void (*entryPoint)(PHGameManager *), void * ud);
inline int PHWMain(int argc, char * argv[], int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    return PHWMain(argc, argv, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), flags, entryPoint, ud);
}

void PHWSetVideoMode(const PHWVideoMode & vm);
PHWVideoMode PHWGetVideoMode();
const vector<PHWVideoMode> & PHWSupportedModes();
void PHWClose();
void PHWSetTitle(const string & title);
void PHWSetVSync(bool vsync);
bool PHWGetVSync();
void PHWSetResizable(bool r);
bool PHWGetResizable();

#endif
