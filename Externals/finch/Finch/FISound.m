#import "FISound.h"
#import "FISoundSample.h"
#import "FIErrorReporter.h"
#import "FIError.h"
#import "FIBuffer.h"

#define CLEAR_ERROR_FLAG alGetError()
#define DETACH_SOURCE 0

@interface FISound ()
@property(assign) ALuint source;
@end

@implementation FISound
@synthesize loop, duration, gain, pitch, source, buffer;

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

#pragma mark Designated Initializer


- (id) initWithBuffer: (FIBuffer*) bff error: (NSError**) error
{
    if (!buffer)
        return nil;

    self = [super init];
    
    ALCcontext *const currentContext = alcGetCurrentContext();
    if (currentContext == NULL)
    {
        NSLog(@"OpenAL context not set, did you initialize Finch?");
        return nil;
    }
        
    // Initialize the source.
    CLEAR_ERROR_FLAG;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, bff.buffer);
    if (![self checkSuccessOrLog:@"Failed to create OpenAL source"])
        return nil;

    buffer = [bff retain];
    
    gain = 1;
    duration = bff.duration;
    return self;
}

- (void) dealloc
{
    [self stop];
    CLEAR_ERROR_FLAG;
    alSourcei(source, AL_BUFFER, DETACH_SOURCE);
    [buffer release];
    alDeleteSources(1, &source), source = 0;
    [self checkSuccessOrLog:@"Failed to clean up after sound"];
    [super dealloc];
}

- (FISound*) duplicate
{
    return [[[FISound alloc] initWithBuffer:buffer error:NULL] autorelease];
}

- (id) initWithSample:(FISoundSample *)sample error:(NSError **)error
{
    return [self initWithBuffer:[[FIBuffer alloc] initWithSample:sample error:error] error:error];
}


#pragma mark Playback Controls

- (void) setGain: (float) val
{
    gain = val;
    alSourcef(source, AL_GAIN, gain);
}

- (void) setPitch: (float) val
{
    pitch = val;
    alSourcef(source, AL_PITCH, pitch);
}

- (BOOL) playing
{
    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

- (void) setLoop: (BOOL) yn
{
    loop = yn;
    alSourcei(source, AL_LOOPING, yn);
}

- (void) play
{
    if (self.playing)
        [self stop];
    CLEAR_ERROR_FLAG;
    alSourcePlay(source);
    [self checkSuccessOrLog:@"Failed to start sound"];
}

- (void) stop
{
    if (!self.playing)
        return;
    CLEAR_ERROR_FLAG;
    alSourceStop(source);
    [self checkSuccessOrLog:@"Failed to stop sound"];
}

@end
