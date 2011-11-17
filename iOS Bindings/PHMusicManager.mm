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

PHMusicManager::PHMusicManager() : currentSound(NULL), m(new PHMutex), currentName(noMusic)
{
    
}

void PHMusicManager::fadeThread(PHThread, args * a)
{    
    NSAutoreleasePool * ap = [[NSAutoreleasePool alloc] init];
    AVAudioPlayer * na = (a->name==noMusic)?nil:[[AVAudioPlayer alloc] initWithContentsOfURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:(a->name).c_str()]] error:nil];
    [na setNumberOfLoops:-1];
    double tm = a->time;
    delete a;
    [na retain];
    
    AVAudioPlayer * oa;
    m->lock();
    oa = currentSound;
    currentSound = na;
    m->unlock();
    if (oa)
    {
        [oa setNumberOfLoops:1];
        double oal = oa.duration-oa.pan;
        if (tm>oal)
            tm = oal;
        double oav = oa.volume;
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
            double ov = oa.volume;
            ov-=ammount*(st/tick);
            if (ov<0) ov = 0;
            oa.volume = ov;
            PHTime::sleep(st);
        }
        [oa stop];
    }
    [na play];
    [oa release];
    [na release];
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