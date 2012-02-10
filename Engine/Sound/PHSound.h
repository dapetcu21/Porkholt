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
class PHEventQueue;
class PHSoundPool;
class PHSound : public PHObject 
{
private:
    PHSoundImpl * impl;
    PHSoundManager * manager;
    PHSound(PHSoundImpl * im);
    PHInvocation inv;
    
    friend class PHSoundManager;
    
    void fireCallback(PHObject * sender, PHEventQueue * timerQueue);
    
    set<PHSoundPool*> soundPool;
    
public:
    void play();
    void stop();
    void pause();
    void resume();
    void playAndCallBack(const PHInvocation & inv, PHEventQueue * timerQueue);
    void playAndRelease(PHEventQueue * timerQueue) { playAndCallBack(PHInv(this,PHObject::release,NULL),timerQueue); }
    bool isPlaying();
    float duration();
    
    bool isLooping();
    void setLooping(bool l);
    
    float gain();
    void setGain(float g);
    
    float pitch();
    void setPitch(float p);
    
    PHSound * duplicate();
    
    void addSoundPool(PHSoundPool * sp);
    void removeSoundPool(PHSoundPool * sp);
    
    ~PHSound();
};

#endif