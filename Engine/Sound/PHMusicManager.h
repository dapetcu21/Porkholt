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

#if defined(PH_IPHONE_OS) && defined(__OBJC__)
@class AVAudioPlayer;
#elif defined(PH_MAC_OS) && defined(__OBJC__)
@class NSSound;
#else
#define NSSound void
#define AVAudioPlayer void
#endif

class PHMusicManager : public PHObject
{
public:
    static const string noMusic;
    static PHMusicManager * singleton();
    
    string currentName;
    
    void setBackgroundMusic(const string & name,ph_float fadeTime, bool loops);
    void setBackgroundMusic(const string & name,ph_float fadeTime) { setBackgroundMusic(name, fadeTime, true); }
    void setBackgroundMusic(const string & name) { setBackgroundMusic(name, 2.0f, true); }
    
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
#if defined(PH_IPHONE_OS) || defined(PH_MAC_OS)
    struct args 
    {
        string name;
        ph_float time;
        bool loops;
    };
    
#ifdef PH_IPHONE_OS
    AVAudioPlayer * currentSound;
#else
    NSSound * currentSound;
#endif
    PHMutex * m;
    void fadeThread(PHThread * sender, args * a);
    void pauseThread(PHThread * sender, void * ud);
    void playThread(PHThread * sender, void * ud);
#endif
};

#endif
