/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSOUNDMANAGER_H
#define PHSOUNDMANAGER_H

#include <Porkholt/Core/PHAnimator.h>

class PHDirectory;
class PHSound;
class PHFile;

class PHSoundManager : public PHAnimator
{
protected:
    PHDirectory * sndDir;
    void * device;
    void * context;
    map<PHHashedString, PHSound*> sounds;
    static bool plugLoaded;
    static void loadPlugins();
    
    set<PHSound*> allsounds;
    set<PHSound*> deletelist;
    set<PHSound*> insertlist;
    bool inside;
    void addSound(PHSound * snd);
    void removeSound(PHSound * snd);

    friend class PHSound;

public:
    PHSoundManager(PHDirectory * dir);
    ~PHSoundManager();

    PHSound * soundNamed(const string & name, PHDirectory * dir);
    PHSound * soundNamed(const string & name) { return soundNamed(name, sndDir); }

    void collectGarbageSounds();

    static map<string, PHAllocator> * extensions;
    static void registerPlugin(const string & extension, PHAllocator a);

    void advanceAnimation(ph_float elapsed);
};

#define PHSOUND_REGISTER_DECODER(extension, clss) PH_REGISTERCLASS(PHSoundManager::extensions, extension, clss)

#endif
