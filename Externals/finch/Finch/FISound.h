@class FISoundSample;
@class FIBuffer;

@interface FISound : NSObject

@property(readonly) float duration;
@property(readonly) BOOL playing;

@property(assign, nonatomic) BOOL loop;
@property(assign, nonatomic) float gain;
@property(assign, nonatomic) float pitch;
@property(retain, nonatomic) FIBuffer * buffer;

- (id) initWithSample: (FISoundSample*) sample error: (NSError**) error;
- (id) initWithBuffer: (FIBuffer*) buffer error: (NSError**) error;

- (void) play;
- (void) stop;

- (FISound*) duplicate;
@end
