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
    
    string currentName;
    
    void setBackgroundMusic(const string & name,double fadeTime);
    void setBackgroundMusic(const string & name) { setBackgroundMusic(name,2.0f); }
    
    PHMusicManager();
    
    bool isPaused() { return paused; }
    void pause();
    void roughPause();
    void play();
    
    void pauseRecursive() { if (!pr) pause(); pr++; }
    void roughPauseRecursive() { if (!pr) roughPause(); pr++; }
    void playRecursive() { if (pr) { pr--; if (!pr) play(); } }
    
private:
    bool paused;
    int pr;
#ifdef PH_IPHONE_OS
    struct args 
    {
        string name;
        double time;
    };
    
    AVAudioPlayer * currentSound;
    PHMutex * m;
    void fadeThread(PHThread * sender, args * a);
    void pauseThread(PHThread * sender, void * ud);
    void playThread(PHThread * sender, void * ud);
#endif
};

#endif
