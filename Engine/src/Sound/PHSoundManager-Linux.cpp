/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Sound/PHSoundManager.h>

PHSoundManager::PHSoundManager(const string & fxpath) : path(fxpath)
{
    
}

PHSoundManager::~PHSoundManager()
{
    for (map<string,PHSound*>::iterator i = sounds.begin(); i!=sounds.end(); i++)
        i->second->release();
}

PHSound * PHSoundManager::soundNamed(const string & name)
{
    map<string,PHSound*>::iterator i = sounds.find(name);
	if (i==sounds.end())
    {
        void * sound = (void*)(size_t)0xff;
        if (!sound) 
        {
            if (name=="placeholder")
                return NULL;
            return soundNamed("placeholder");
        }
        PHSound * snd = new PHSound(sound, this);
        sounds.insert(make_pair<string,PHSound*>(name,snd));
        return snd;
    }
    return i->second;
}
