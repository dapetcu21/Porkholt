/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Core/PHEventQueue.h>
#include <Porkholt/Core/PHTimer.h>
#include <Porkholt/Sound/PHSoundPool.h>

PHSound::PHSound(void * im, PHSoundManager * man) : impl(im), manager(man)
{
}

void PHSound::play()
{
}

void PHSound::pause()
{
}

void PHSound::stop()
{
}

void PHSound::resume()
{
}

void PHSound::fireCallback(PHObject * sender, PHEventQueue * timerQueue)
{
    inv.call();
}

void PHSound::playAndCallBack(const PHInvocation & invo, PHEventQueue * timerQueue)
{
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
    return false;
}

float PHSound::duration()
{
    return 0;
}

bool PHSound::isLooping()
{
    return false;
}

void PHSound::setLooping(bool l)
{
}

float PHSound::gain()
{
    return 1;
}

void PHSound::setGain(float g)
{
}


float PHSound::pitch()
{
    return 1;
}

void PHSound::setPitch(float p)
{
}


PHSound * PHSound::duplicate()
{
    return new PHSound(impl, manager);
}

PHSound::~PHSound()
{
    set<PHSoundPool*> sp = soundPool;
    for (set<PHSoundPool*>::iterator i = sp.begin(); i != sp.end(); i++)
        (*i)->removeSound(this);
}
