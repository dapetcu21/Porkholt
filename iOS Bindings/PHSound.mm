//
//  PHSound.mm
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHSound.h"
#include "PHSoundManager.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#include "FISound.h"
#include "PHEventQueue.h"
#include "PHTimer.h"
#include "PHSoundPool.h"

PHSound::PHSound(FISound * im) : impl(im)
{
    [impl retain];
}

void PHSound::play()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl play];
    [pool drain];
}

void PHSound::pause()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl pause];
    [pool drain];
}

void PHSound::stop()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl stop];
    [pool drain];
}

void PHSound::resume()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl resume];
    [pool drain];
}

void PHSound::fireCallback(PHObject * sender, PHEventQueue * timerQueue)
{
    if (!impl.playing) 
    {
        inv.call();
        return;
    }
    double tm = (impl.duration-impl.time)*impl.pitch;
    PHTimer * timer = new PHTimer;
    timer->setTimeInterval(tm);
    timer->setCallsBackOnInvalidate(true);
    timer->setCallback(PHInv(this,PHSound::fireCallback,timerQueue));
    timerQueue->scheduleTimer(timer,this);
    timer->release();
}

void PHSound::playAndCallBack(PHInvocation invo, PHEventQueue * timerQueue)
{
    inv = invo;
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl play];
    [pool drain];
    fireCallback(NULL,timerQueue);
}

void PHSound::addSoundPool(PHSoundPool * sp)
{
    soundPool.insert(sp);
}

void PHSound::removeSoundPool(PHSoundPool * sp)
{
    soundPool.erase(sp);
}

bool PHSound::isPlaying()
{
    return impl.playing;
}

float PHSound::duration()
{
    return impl.duration;
}

bool PHSound::isLooping()
{
    return impl.loop;
}

void PHSound::setLooping(bool l)
{
    impl.loop = l;
}

float PHSound::gain()
{
    return impl.gain;
}

void PHSound::setGain(float g)
{
    impl.gain = g;
}

float PHSound::pitch()
{
    return impl.pitch;
}

void PHSound::setPitch(float p)
{
    impl.pitch = p;
}

PHSound * PHSound::duplicate()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init]; 
    PHSound * s = new PHSound([impl duplicate]);
    [pool drain];
    return s;
}

PHSound::~PHSound()
{
    set<PHSoundPool*> sp = soundPool;
    for (set<PHSoundPool*>::iterator i = sp.begin(); i != sp.end(); i++)
        (*i)->removeSound(this);
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl release];
    [pool drain];
}
