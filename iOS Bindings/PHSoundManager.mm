//
//  PHSoundManager.mm
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHSoundManager.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import "FISoundEngine.h"
#import "FIFactory.h"
#import "PHFileManager.h"

map<string,PHSound*> PHSoundManager::sounds;
PHSoundManager * PHSoundManager::sgl = NULL;

PHSoundManager::PHSoundManager()
{
    soundFactory = [[FIFactory alloc] init];
    [soundFactory setLogger:FILoggerNSLog];
    [soundFactory setSoundBundle:[NSBundle bundleWithPath:[NSString stringWithUTF8String:(PHFileManager::resourcePath()+"/snd/fx/").c_str()]]];
    
    soundEngine = [soundFactory buildSoundEngine];
    [soundEngine activateAudioSessionWithCategory:AVAudioSessionCategoryPlayback];
    [soundEngine openAudioDevice];
    [soundEngine retain];
}

PHSoundManager::~PHSoundManager()
{
    [soundFactory release];
    [soundEngine release];
}

PHSound * PHSoundManager::soundNamed(const string & name)
{
    map<string,PHSound*>::iterator i = sounds.find(name);
	if (i==sounds.end())
    {
        FISound * sound = [soundFactory loadSoundNamed:[NSString stringWithUTF8String:(name+".wav").c_str()]];
        if (!sound) 
        {
            if (name=="placeholder")
                return NULL;
            return soundNamed("placeholder");
        }
        PHSound * snd = new PHSound(sound);
        sounds.insert(make_pair<string,PHSound*>(name,snd));
        return snd;
    }
    return i->second;
}