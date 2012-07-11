/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSOUNDMANAGER_H
#define PHSOUNDMANAGER_H

#include <Porkholt/Core/PHMain.h>

class PHDirectory;
class PHSound;
class PHFile;

class PHSoundManager : public PHObject
{
protected:
    PHDirectory * sndDir;
    void * device;
    void * context;
    map<PHHashedString, PHSound*> sounds;
    static bool plugLoaded;
    static void loadPlugins();

public:
    PHSoundManager(PHDirectory * dir);
    ~PHSoundManager();

    PHSound * soundNamed(const string & name, PHDirectory * dir);
    PHSound * soundNamed(const string & name) { return soundNamed(name, sndDir); }

    void collectGarbageSounds();

    static map<string, PHAllocator> * extensions;
    static void registerPlugin(const string & extension, PHAllocator a);
};

#define PHSOUND_REGISTER_DECODER(extension, clss) PH_REGISTERCLASS(PHSoundManager::extensions, extension, clss)

#endif
