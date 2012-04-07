#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h> 
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif

enum SampleEndianity
{
    kEndianityUnknown = -1,
    kLittleEndian,
    kBigEndian
};

@interface FISoundSample : NSObject {}

@property(assign) int channels;
@property(assign) int bitsPerChannel;
@property(assign) int sampleRate;
@property(assign) int endianity;
@property(assign) float duration;
@property(retain) NSData *data;

@end
