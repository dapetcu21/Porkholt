//
//  PHSoundManager.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHSOUNDMANAGER_H
#define PHSOUNDMANAGER_H

#include "PHMain.h"
#include "PHSound.h"

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
private:
    static PHSoundManager * sgl;
    
    PHSoundManager();
    ~PHSoundManager();
    
    static map<string,PHSound*> sounds;
    
    
#ifdef PH_IPHONE_OS
    FISoundEngine * soundEngine;
    FIFactory * soundFactory;
#endif
public:
    static PHSoundManager * singleton() {
        if (!sgl)
            sgl = new PHSoundManager();
        return sgl;
    }
    PHSound * soundNamed(const string & name);
};

#endif
