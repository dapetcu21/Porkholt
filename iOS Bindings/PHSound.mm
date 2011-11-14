//
//  PHSound.mm
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHSound.h"
#include "PHSoundManager.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#include "FISound.h"

PHSound::PHSound(FISound * im) : impl(im)
{
    [impl retain];
}

void PHSound::play()
{
    [impl play];
}

PHSound::~PHSound()
{
    [impl release];
}
