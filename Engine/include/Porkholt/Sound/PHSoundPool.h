/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSOUNDPOOL_H
#define PHSOUNDPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHSound;
class PHSoundPool : public PHObject
{
private:
    set<PHSound*> sounds;
    set<PHSound*> paused;
public:
    void addSound(PHSound * snd);
    void removeSound(PHSound * snd);
    
    void play();
    void stop();
    void pause();
    void resume();
    
    void setLooping(bool l);
    void setGain(float g);
    void setPitch(float p);
    
    ~PHSoundPool();
};


#endif
