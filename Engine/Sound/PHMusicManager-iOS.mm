/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
    
    ph_float tm = 1.0f;
    ph_float pos = ca.currentTime;
    ph_float oal = ca.duration-pos;
    if (tm>oal)
        tm = oal;
    ph_float oav = ca.volume;
    m->unlock();
    static const ph_float tick = 0.1;
    ph_float ammount = oav/(tm/tick);
    while (tm)
    {
        ph_float st = tick;
        if (tm<st)
        {
            st = tm;
            tm = 0;
        } else
            tm-= st;
        m->lock();
        ph_float ov = ca.volume;
        ov-=ammount*(st/tick);
        if (ov>1) ov = 1;
        ca.volume = ov;
        if (!paused)
        {
            m->unlock();
            break;
        }
        m->unlock();
        PHTime::sleep(st);
    }
    m->lock();
    if (paused)
    {
        ca.currentTime = pos;
        [ca pause];
    }
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
    
    ph_float tm = 1.0f;
    ca.volume = 0;
    static const ph_float tick = 0.1;
    ph_float ammount = 1.0f/(tm/tick);
    bool alreadyplaying = ca.playing;
    if (!alreadyplaying)
    {
        [ca play];
        NSTimeInterval pos = ca.currentTime;
        ph_float du = ca.duration;
        pos-=tm;
        while (du && pos<0)
            pos+=du;
        ca.currentTime = pos;
    }
    m->unlock();
    while (tm)
    {
        ph_float st = tick;
        if (tm<st)
        {
            st = tm;
            tm = 0;
        } else
            tm-= st;
        m->lock();
        ph_float ov = ca.volume;
        ov+=ammount*(st/tick);
        if (ov>1) ov = 1;
        ca.volume = ov;
        if (paused)
        {
            m->unlock();
            break;
        }
        m->unlock();
        PHTime::sleep(st);
    }
    m->lock();
    [ca release];
    m->unlock();
}

void PHMusicManager::fadeThread(PHThread * sender, args * a)
{    
    NSAutoreleasePool * ap = [[NSAutoreleasePool alloc] init];
#ifdef PH_SIMULATOR
    a->name = noMusic;
#endif
    AVAudioPlayer * na = (a->name==noMusic)?nil:[[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:(a->name).c_str()]] error:nil];
    [na setNumberOfLoops:a->loops?-1:1];
    [na prepareToPlay];
    ph_float tm = a->time;
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
        ph_float oal = oa.duration-oa.currentTime;
        if (tm>oal)
            tm = oal;
        ph_float oav = oa.volume;
        m->unlock();
        static const ph_float tick = 0.1;
        ph_float ammount = oav/(tm/tick);
        while (tm)
        {
            ph_float st = tick;
            if (tm<st)
            {
                st = tm;
                tm = 0;
            } else
                tm-= st;
            m->lock();
            ph_float ov = oa.volume;
            ov-=ammount*(st/tick);
            if (ov<0) ov = 0;
            oa.volume = ov;
            m->unlock();
            PHTime::sleep(st);
        }
        [oa stop];
    }
    m->lock();
    if (paused)
    {
        paused = false;
        pr = 0;
    }
    [na play];
    [oa release];
    [na release];
    m->unlock();
    [ap drain];
}

void PHMusicManager::setBackgroundMusic(const string & name,ph_float fadeTime, bool loop)
{
    if (name==currentName) return;
    currentName = name;
    m->lock();
    PHThread * t = new PHThread;
    args * a = new args;
    a->name = name;
    a->time = fadeTime;
    a->loops = loop;
    t->setFunction(PHInv(this,PHMusicManager::fadeThread,a));
    t->setAutoRelease(true);
    t->start();
    m->unlock();
}
