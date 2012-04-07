#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h> 
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif

@class FISoundSample;

@protocol FISoundDecoder <NSObject>

- (NSSet*) supportedFileExtensions;
- (FISoundSample*) decodeFileAtPath: (NSString*) path error: (NSError**) error;

@end
