//
//  PHMusicManager.mm
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHMusicManager.h"
#include "PHFileManager.h"
#include <AVFoundation/AVFoundation.h>

PHMusicManager * PHMusicManager::singleton()
{
    static PHMusicManager * man = NULL;
    if (!man)
        man = new PHMusicManager();
        return man;
}

const string PHMusicManager::noMusic("<null>");

PHMusicManager::PHMusicManager() : currentSound(NULL), m(new PHMutex), currentName(noMusic), pr(0), paused(false)
{
    PHMessage::messageWithName("appSuspended")->addListener(this, (PHCallback)&PHMusicManager::roughPauseRecursive);
    PHMessage::messageWithName("appResumed")->addListener(this, (PHCallback)&PHMusicManager::playRecursive);
}

void PHMusicManager::pause()
{
    m->lock();
    if (paused) 
    {
        m->unlock();
        return;
    }
    paused = true;
    PHLog("pause");
    if (currentSound)
    {
        PHThread * t = new PHThread;
        t->setFunction(PHInv(this,PHMusicManager::pauseThread,NULL));
        t->setAutoRelease(true);
        t->start();
    }
    m->unlock();
}

void PHMusicManager::roughPause()
{
    m->lock();
    if (paused) 
    {
        m->unlock();
        return;
    }
    paused = true;
    PHLog("pause");
    [currentSound pause];
    m->unlock();
}

void PHMusicManager::play()
{
    m->lock();
    if (!paused) 
    {
        m->unlock();
        return;
    }
    paused = false;
    PHLog("play");
    if (currentSound)
    {
        PHThread * t = new PHThread;
        t->setFunction(PHInv(this,PHMusicManager::playThread,NULL));
        t->setAutoRelease(true);
        t->start();
    }
    m->unlock();
}

void PHMusicManager::pauseThread(PHThread * sender, void * ud)
{
    m->lock();
    AVAudioPlayer * ca = currentSound;
    if (!ca || !ca.playing)
    {
        m->unlock();
        return;
    }
    [ca retain];
    
    double tm = 1.0f;
    double pos = ca.currentTime;
    double oal = ca.duration-pos;
    if (tm>oal)
        tm = oal;
    double oav = ca.volume;
    m->unlock();
    PHLog("hii %d",ca.playing);
    static const double tick = 0.1;
    double ammount = oav/(tm/tick);
    while (tm)
    {
        double st = tick;
        if (tm<st)
        {
            st = tm;
            tm = 0;
        } else
            tm-= st;
        m->lock();
        double ov = ca.volume;
        ov-=ammount*(st/tick);
        if (ov>1) ov = 1;
        ca.volume = ov;
        m->unlock();
        PHLog("%lf",ov);
        PHTime::sleep(st);
    }
    m->lock();
    if (paused)
    {
        ca.currentTime = pos;
        [ca pause];
    }
    PHLog("hii %d",ca.playing);
    [ca release];
    m->unlock();
}

void PHMusicManager::playThread(PHThread * sender, void * ud)
{
    m->lock();
    AVAudioPlayer * ca = currentSound;
    if (!ca)
    {
        m->unlock();
        return;
    }
    [ca retain];
    
    double tm = 1.0f;
    ca.volume = 0;
    static const double tick = 0.1;
    double ammount = 1.0f/(tm/tick);
    bool alreadyplaying = ca.playing;
    if (!alreadyplaying)
    {
        [ca play];
        NSTimeInterval pos = ca.currentTime;
        double du = ca.duration;
        pos-=tm;
        while (du && pos<0)
            pos+=du;
        ca.currentTime = pos;
    }
    m->unlock();
    while (tm)
    {
        double st = tick;
        if (tm<st)
        {
            st = tm;
            tm = 0;
        } else
            tm-= st;
        m->lock();
        double ov = ca.volume;
        ov+=ammount*(st/tick);
        if (ov>1) ov = 1;
        ca.volume = ov;
        m->unlock();
        PHLog("%lf",ov);
        PHTime::sleep(st);
    }
    m->lock();
    [ca release];
    m->unlock();
}

void PHMusicManager::fadeThread(PHThread * sender, args * a)
{    
    NSAutoreleasePool * ap = [[NSAutoreleasePool alloc] init];
    AVAudioPlayer * na = (a->name==noMusic)?nil:[[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:(a->name).c_str()]] error:nil];
    [na setNumberOfLoops:-1];
    [na prepareToPlay];
    double tm = a->time;
    delete a;
    [na retain];
    
    AVAudioPlayer * oa;
    bool p;
    m->lock();
    oa = currentSound;
    currentSound = na;
    p = paused;
    m->unlock();
    if (oa && !p)
    {
        m->lock();
        [oa setNumberOfLoops:1];
        double oal = oa.duration-oa.currentTime;
        if (tm>oal)
            tm = oal;
        double oav = oa.volume;
        m->unlock();
        static const double tick = 0.1;
        double ammount = oav/(tm/tick);
        while (tm)
        {
            double st = tick;
            if (tm<st)
            {
                st = tm;
                tm = 0;
            } else
                tm-= st;
            m->lock();
            double ov = oa.volume;
            ov-=ammount*(st/tick);
            if (ov<0) ov = 0;
            oa.volume = ov;
            m->unlock();
            PHLog("%lf",ov);
            PHTime::sleep(st);
        }
        [oa stop];
    }
    m->lock();
    if (!paused)
        [na play];
    [oa release];
    [na release];
    m->unlock();
    [ap drain];
}

string PHMusicManager::musicNamed(const string & name)
{
    return PHFileManager::resourcePath()+"/snd/music/"+name+".mp3";
}

void PHMusicManager::setBackgroundMusic(const string & name,double fadeTime)
{
    if (name==currentName) return;
    currentName = name;
    
    m->lock();
    PHThread * t = new PHThread;
    args * a = new args;
    a->name = name;
    a->time = fadeTime;
    t->setFunction(PHInv(this,PHMusicManager::fadeThread,a));
    t->setAutoRelease(true);
    t->start();
    m->unlock();
}