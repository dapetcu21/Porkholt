/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */


#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h> 
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif

#import <Foundation/Foundation.h>

@class FISoundSample;

@interface FIBuffer : NSObject
@property(readonly) ALuint buffer;
@property(readonly) float duration;

- (id) initWithSample: (FISoundSample*) sample error: (NSError**) error;

@end
