/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "FIBuffer.h"
#import "FIErrorReporter.h"
#import "FISoundSample.h"
#import "FIError.h"

#define CLEAR_ERROR_FLAG alGetError()

@implementation FIBuffer
@synthesize buffer,duration;

// Clears the error flag.
- (BOOL) checkSuccessOrLog: (NSString*) msg
{
    ALenum errcode;
    if ((errcode = alGetError()) != AL_NO_ERROR)
    {
        NSLog(@"%@, error code %x.", msg, errcode);
        return NO;
    }
    return YES;
}

- (id) initWithData: (const ALvoid*) data size: (ALsizei) size
             format: (ALenum) format sampleRate: (ALsizei) frequency
           duration: (float) seconds
{
    self = [super init];
    
    duration = seconds;
    
    ALCcontext *const currentContext = alcGetCurrentContext();
    if (currentContext == NULL)
    {
        NSLog(@"OpenAL context not set, did you initialize Finch?");
        return nil;
    }
    
    // Allocate buffer.
    CLEAR_ERROR_FLAG;
    alGenBuffers(1, &buffer);
    if (![self checkSuccessOrLog:@"Failed to allocate OpenAL buffer"])
        return nil;
    
    // Pass sound data to OpenAL.
    CLEAR_ERROR_FLAG;
    alBufferData(buffer, format, data, size, frequency);
    if (![self checkSuccessOrLog:@"Failed to fill OpenAL buffers"])
        return nil;
    
    return self;
}

- (id) initWithSample: (FISoundSample*) sample error: (NSError**) error
{
    if (!sample)
        return nil;
    
    FIErrorReporter *reporter = [FIErrorReporter forDomain:@"Sound Initialization" error:error];
    
    // Check the number of channels
    if (sample.channels != 1 && sample.channels != 2) {
        *error = [reporter errorWithCode:FIErrorInvalidNumberOfChannels];
        return nil;
    }
    
    // Check sample resolution
    if (sample.bitsPerChannel != 8 && sample.bitsPerChannel != 16) {
        *error = [reporter errorWithCode:FIErrorInvalidSampleResolution];
        return nil;
    }
    
    // Check data endianity
    if (sample.endianity != kLittleEndian) {
        *error = [reporter errorWithCode:FIErrorInvalidEndianity];
        return nil;
    }
    
    const ALenum format = sample.channels == 1 ?
    (sample.bitsPerChannel == 16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8) :
    (sample.bitsPerChannel == 16 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8);
    return [self initWithData:sample.data.bytes size:sample.data.length
                       format:format sampleRate:sample.sampleRate duration:sample.duration];
}

- (void) dealloc
{
    alDeleteBuffers(1, &buffer), buffer = 0;
    [super dealloc];
}

@end
