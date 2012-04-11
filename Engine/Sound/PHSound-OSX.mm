//
//  PHSound.mm
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHSound.h"
#include "PHSoundManager.h"
#import <AppKit/AppKit.h>
#include "PHEventQueue.h"
#include "PHTimer.h"
#include "PHSoundPool.h"

@interface PHCSound : NSObject<NSSoundDelegate>
{
	PHSound * phsound;
}
@property(nonatomic, assign) PHSound * phsound;
-(void)sound:(NSSound *)sound didFinishPlaying:(BOOL)finishedPlaying;
@end

@implementation PHCSound
@synthesize phsound;
-(void)sound:(NSSound *)sound didFinishPlaying:(BOOL)finishedPlaying
{
	if (phsound)
		phsound->fireCallback(NULL, NULL);
}
@end

PHSound::PHSound(NSSound * im, PHSoundManager * man) : impl(im), manager(man)
{
    [impl retain];
    del = [[PHCSound alloc] init];
	impl.delegate = del;
    del.phsound = this;
}

void PHSound::play()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl play];
    inv.clear();
    [pool drain];
}

void PHSound::pause()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl pause];
    [pool drain];
}

void PHSound::stop()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl stop];
    [pool drain];
}

void PHSound::resume()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl resume];
    [pool drain];
}

void PHSound::fireCallback(PHObject * sender, PHEventQueue * timerQueue)
{
    inv.call();
}

void PHSound::playAndCallBack(const PHInvocation & invo, PHEventQueue * timerQueue)
{
    inv = invo;
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl play];
    [pool drain];
}

void PHSound::addSoundPool(PHSoundPool * sp)
{
    soundPool.insert(sp);
}

void PHSound::removeSoundPool(PHSoundPool * sp)
{
    soundPool.erase(sp);
}

bool PHSound::isPlaying()
{
    return impl.isPlaying;
}

float PHSound::duration()
{
    return impl.duration;
}

bool PHSound::isLooping()
{
    return impl.loops;
}

void PHSound::setLooping(bool l)
{
    impl.loops = l;
}

float PHSound::gain()
{
    return impl.volume;
}

void PHSound::setGain(float g)
{
    impl.volume = g;
}


float PHSound::pitch()
{
//    return impl.pitch;
    return 1;
}

void PHSound::setPitch(float p)
{
//    impl.pitch = p;
}


PHSound * PHSound::duplicate()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init]; 
    PHSound * s = new PHSound([[impl copy] autorelease],manager);
    [pool drain];
    return s;
}

PHSound::~PHSound()
{
    set<PHSoundPool*> sp = soundPool;
    for (set<PHSoundPool*>::iterator i = sp.begin(); i != sp.end(); i++)
        (*i)->removeSound(this);
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [impl release];
    [del release];
    [pool drain];
}
