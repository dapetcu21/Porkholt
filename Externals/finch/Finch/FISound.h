#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h> 
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif

@class FISoundSample;
@class FIBuffer;

@interface FISound : NSObject

@property(readonly) float duration;
@property(readonly) BOOL playing;
@property(readonly) BOOL paused;

@property(assign, nonatomic) BOOL loop;
@property(assign, nonatomic) float gain;
@property(assign, nonatomic) float pitch;
@property(retain, nonatomic) FIBuffer * buffer;
@property(assign, nonatomic) float time;

- (id) initWithSample: (FISoundSample*) sample error: (NSError**) error;
- (id) initWithBuffer: (FIBuffer*) buffer error: (NSError**) error;

- (void) play;
- (void) stop;
- (void) pause;
- (void) resume;

- (FISound*) duplicate;
@end
