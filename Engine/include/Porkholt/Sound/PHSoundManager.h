/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSOUNDMANAGER_H
#define PHSOUNDMANAGER_H

#include <Porkholt/Core/PHAnimator.h>
#include <Porkholt/Core/PHCinematicActor.h>

class PHDirectory;
class PHSound;
class PHFile;

class PHSoundManager : public PHAnimator, public PHCinematicActor
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

    PHSound * music;

    friend class PHSound;

public:
    PHSoundManager(PHDirectory * dir, PHGameManager * gm);
    ~PHSoundManager();

    PHGameManager * gameManager() { return actorGameManager(); }

    PHSound * soundNamed(const string & name, PHDirectory * dir);
    PHSound * soundNamed(const string & name) { return soundNamed(name, sndDir); }

    void collectGarbageSounds();

    static map<string, PHAllocator> * extensions;
    static void registerPlugin(const string & extension, PHAllocator a);

    void advanceAnimation(float elapsed);

    PHSound * backgroundMusic() { return music; }
    void setBackgroundMusic(PHSound * music);

    void setGain(float gain);
    float gain();
    void setPositiion(const PHVector3 & p);
    PHVector3 position();
    void setVelocity(const PHVector3 & p);
    PHVector3 velocity();
    void setOrientation(const PHQuaternion & p);
    void setOrientation(const PHVector3 & at, const PHVector3 & up);
    pair<PHVector3, PHVector3> orientation();

    float dopplerFactor();
    void setDopplerFactor(float v);
    float speedOfSound();
    void setSpeedOfSound(float v);

    enum distModel
    {
        inverseDistance,
        inverseDistanceClamped,
        linearDistance,
        linearDistanceClamped,
        exponentDistance,
        exponentDistanceClamped,
        none,
        NUMDISTMODELS
    };

    enum distModel distanceModel();
    void setDistanceModel(enum distModel model);
};

#define PHSOUND_REGISTER_DECODER(extension, clss) PH_REGISTERCLASS(PHSoundManager::extensions, extension, clss)

#endif
