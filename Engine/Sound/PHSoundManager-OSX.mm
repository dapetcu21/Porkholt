/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHSoundManager.h"
#import <AppKit/AppKit.h>
#import "PHFileManager.h"

PHSoundManager::PHSoundManager(const string & fxpath) : path(fxpath)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
   
    [pool drain];
}

PHSoundManager::~PHSoundManager()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    for (map<string,PHSound*>::iterator i = sounds.begin(); i!=sounds.end(); i++)
        i->second->release();
    [pool drain];
}

PHSound * PHSoundManager::soundNamed(const string & name)
{
    map<string,PHSound*>::iterator i = sounds.find(name);
	if (i==sounds.end())
    {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        NSSound * sound = [[NSSound alloc ] initWithContentsOfFile:[NSString stringWithUTF8String:(path+"/"+name+".wav").c_str()] byReference:YES];
        [sound retain];
        [pool drain];
        if (!sound) 
        {
            if (name=="placeholder")
                return NULL;
            return soundNamed("placeholder");
        }
        PHSound * snd = new PHSound(sound, this);
        [sound release];
        sounds.insert(make_pair<string,PHSound*>(name,snd));
        return snd;
    }
    return i->second;
}
