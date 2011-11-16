//
//  PHSoundPool.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHSOUNDPOOL_H
#define PHSOUNDPOOL_H

#include "PHMain.h"

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
