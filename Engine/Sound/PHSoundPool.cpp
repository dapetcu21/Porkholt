/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHSoundPool.h"
#include "PHSound.h"

#define mapmethod(meth) void PHSoundPool::meth() { for (set<PHSound*>::iterator i = sounds.begin(); i != sounds.end(); i++) (*i)->meth(); }
#define mapsetter(meth,type) void PHSoundPool::meth(type x) { for (set<PHSound*>::iterator i = sounds.begin(); i != sounds.end(); i++) (*i)->meth(x); }

mapmethod(play)
mapmethod(pause)
mapmethod(resume)
mapmethod(stop)
mapsetter(setLooping,bool)
mapsetter(setGain,float)
mapsetter(setPitch,float)

void PHSoundPool::addSound(PHSound * snd)
{
    if (!snd) return;
    if (sounds.insert(snd).second)
        snd->addSoundPool(this);
}

void PHSoundPool::removeSound(PHSound * snd)
{
    if (!snd) return;
    if (sounds.erase(snd))
        snd->removeSoundPool(this);
}

PHSoundPool::~PHSoundPool()
{
    set<PHSound*> s = sounds;
    for (set<PHSound*>::iterator i = s.begin(); i != s.end(); i++)
        (*i)->removeSoundPool(this);
}
