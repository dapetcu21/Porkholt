/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSOUNDMANAGER_H
#define PHSOUNDMANAGER_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Sound/PHSound.h>

#ifdef PH_IPHONE_OS
#ifdef __OBJC__
@class FISoundEngine;
@class FIFactory;
#else
#define FISoundEngine void
#define FIFactory void
#endif
#endif

class PHSoundManager : public PHObject
{
public:
    PHSoundManager(const string & fxpath);
    ~PHSoundManager();
    
private:
    map<string,PHSound*> sounds;
    string path;
    
    
#ifdef PH_IPHONE_OS
    FISoundEngine * soundEngine;
    FIFactory * soundFactory;
#endif
public:
    PHSound * soundNamed(const string & name);
};

#endif