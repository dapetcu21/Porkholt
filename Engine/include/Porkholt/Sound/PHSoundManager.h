/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>

class PHDirectory;
class PHSound;
class PHFile;

struct ALCdevice;
struct ALCcontext;

class PHSoundManager : public PHObject
{
protected:
    PHDirectory * sndDir;
    ALCdevice * device;
    ALCcontext * context;
    map<PHHashedString, PHSound*> sounds;
    
    static map<string, PHAllocator> * extensions;
public:
    PHSoundManager(PHDirectory * dir);
    ~PHSoundManager();

    PHSound * soundNamed(const string & name, PHDirectory * dir);
    PHSound * soundNamed(const string & name) { return soundNamed(name, sndDir); }

    void collectGarbageSounds();

};

#define PHSOUND_REGISTER_EXTENSION(extension, clss) PH_REGISTERCLASS(PHSoundManager::extensions, extension, clss)
