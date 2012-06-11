/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHMusicManager.h>
#include <Porkholt/IO/PHFileManager.h>

PHMusicManager * PHMusicManager::singleton()
{
    static PHMusicManager * man = NULL;
    if (!man)
        man = new PHMusicManager();
        return man;
}

const string PHMusicManager::noMusic("<null>");

PHMusicManager::PHMusicManager() : currentName(noMusic), pr(0), paused(false)
{
    PHMessage::messageWithName("appSuspended")->addListener(this, (PHCallback)&PHMusicManager::roughPauseRecursive);
    PHMessage::messageWithName("appResumed")->addListener(this, (PHCallback)&PHMusicManager::playRecursive);
}

void PHMusicManager::pause()
{
    if (paused) 
        return;
    paused = true;
}

void PHMusicManager::roughPause()
{
    if (paused) 
        return;
    paused = true;
}

void PHMusicManager::play()
{
    if (!paused) 
        return;
    paused = false;
}

void PHMusicManager::setBackgroundMusic(const string & name, ph_float fadeTime, bool loops)
{
    if (name==currentName) return;
    currentName = name;
}
