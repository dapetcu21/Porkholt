//
//  PHStartGame.h
//  Porkholt
//
//  Created by Marius Petcu on 2/7/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHSTARTGAME_H
#define PHSTARTGAME_H

#include "PHMain.h"
#include "PHGameManager.h"

enum PHStartGame_flags
{
    PHStartGame_GLES1 = 1<<0,
    PHStartGame_GLES2 = 1<<1,
    PHStartGame_frameAnimation = 1<<2,
    PHStartGame_30FPS = 1<<3,
    PHStartGame_FullScreen = 1<<4,
    PHStartGame_Resizable = 1<<5
};

int PHStartGame(int argc, char * argv[], unsigned int resolutionX, unsigned int resolutionY, int flags, void (*entryPoint)(PHGameManager *), void * ud);
inline int PHStartGame(int argc, char * argv[], int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    return PHStartGame(argc, argv, 800, 600, flags, entryPoint, ud);
}

extern void * PHMainWindow;
void * PHCreateWindow(const string & title, unsigned int resolutionX, unsigned int resolutionY, int flags, void (*entryPoint)(PHGameManager *), void * ud);
void PHResizeWindow(void * window, unsigned int resolutionX, unsigned int resolutionY);
void PHWindowSetFullScreen(void * window, bool fullScreen);
void PHCloseWindow(void * window);
void PHWindowSetTitle(void * window, const string & title);

#endif
