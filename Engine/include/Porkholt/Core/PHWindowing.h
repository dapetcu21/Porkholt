/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHWINDOWING_H
#define PHWINDOWING_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHGameManager.h>

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

#ifdef PH_ANDROID
#define PHMAIN ANativeActivity_onCreate 
#define PHMAIN_PROTO void* activity, void* savedState, size_t savedStateSize
#define PHMAIN_ARGS activity, savedState, savedStateSize
#define PHMAIN_RETURN(x) x
#define PHMAIN_TYPE void
#define PHMAIN_DEFINE extern "C" PHMAIN_TYPE PHMAIN(PHMAIN_PROTO)

#elif defined(PH_LIVEPAPERS)

#define PHMAIN LPInitViewController
#define PHMAIN_PROTO void * infoDictionary
#define PHMAIN_ARGS infoDictionary
#define PHMAIN_RETURN(x) return (x)
#define PHMAIN_TYPE void *
#define PHMAIN_DEFINE extern "C" __attribute__((visibility("default"))) PHMAIN_TYPE PHMAIN(PHMAIN_PROTO)

#else
#define PHMAIN main
#define PHMAIN_PROTO int argc, char * argv[]
#define PHMAIN_ARGS argc, argv
#define PHMAIN_RETURN(x) return (x)
#define PHMAIN_TYPE int
#define PHMAIN_DEFINE PHMAIN_TYPE PHMAIN(PHMAIN_PROTO)
#endif

#define PHMAIN_DECLARE(...) PHMAIN_DEFINE { PHMAIN_RETURN(PHWMain(PHMAIN_ARGS, __VA_ARGS__)); }

PHMAIN_TYPE PHWMain(PHMAIN_PROTO, const PHWVideoMode & vmode, int flags, void (*entryPoint)(PHGameManager *), void * ud);
inline PHMAIN_TYPE PHWMain(PHMAIN_PROTO, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHMAIN_RETURN(PHWMain(PHMAIN_ARGS, PHWVideoMode(800, 600, 60, PHWVideoMode::Windowed), flags, entryPoint, ud));
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

inline void PHWApplyFlags(PHGameManager * gm, int flags)
{
    gm->setUsesRemote((flags & PHWRemote) != 0);
    gm->setShowsFPS((flags & PHWShowFPS) != 0);
    gm->setFrameAnimation((flags & PHWFrameAnimation) != 0);
    gm->setFpsCapped((flags & PHWVSync) != 0);
}

//LivePapers functions
class PHImage;
PHImage * LPPHGetWallpaperImage(PHGameManager * gm);
PHRect LPPHGetWallpaperBounds(PHGameManager * gm);
void LPPHSetFPS(PHGameManager * gm, int fps, int stage = 0);
void LPPHClearWallpaperImage(PHGameManager * gm);

#endif
