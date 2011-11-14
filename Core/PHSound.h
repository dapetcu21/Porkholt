//
//  PHSound.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHSOUND_H
#define PHSOUND_H

#include "PHMain.h"


#if defined(PH_IPHONE_OS) && defined(__OBJC__)
@class FISound;
#define PHSoundImpl FISound
#else
#define PHSoundImpl void
#endif


class PHSoundManager;
class PHSound : public PHObject 
{
private:
    PHSoundImpl * impl;
    PHSoundManager * manager;
    PHSound(PHSoundImpl * im);
    
    friend class PHSoundManager;
public:
    void play();
    ~PHSound();
};

#endif
