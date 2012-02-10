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
    PHStartGame_30FPS = 1<<3
};

int PHStartGame(int argc, char * argv[], int flags, void (*entryPoint)(PHGameManager *), void * ud);

#endif
