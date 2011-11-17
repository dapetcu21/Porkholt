//
//  PHMusicManager.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHMUSICMANAGER_H
#define PHMUSICMANAGER_H

#include "PHMain.h"

#ifdef PH_IPHONE_OS
#ifdef __OBJC__
@class AVAudioPlayer;
#else
#define AVAudioPlayer void
#endif
#endif

class PHMusicManager : public PHObject
{
public:
    static const string noMusic;
    static PHMusicManager * singleton();
    
    static string musicNamed(const string & name);
    
    string currentName;
    
    void setBackgroundMusic(const string & name,double fadeTime);
    void setBackgroundMusic(const string & name) { setBackgroundMusic(name,2.0f); }
    
    PHMusicManager();
    
private:
#ifdef PH_IPHONE_OS
    struct args 
    {
        string name;
        double time;
    };
    
    AVAudioPlayer * currentSound;
    PHMutex * m;
    void fadeThread(PHThread, args * a);
#endif
};

#endif
