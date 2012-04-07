
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h> 
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif
@interface FIErrorReporter : NSObject
{
    NSString *domain;
}

- (id) initWithDomain: (NSString*) errDomain error: (NSError**) error;
+ (id) forDomain: (NSString*) errDomain error: (NSError**) error;

- (NSError*) errorWithCode: (int) code;
- (NSError*) errorWithCode: (int) code description: (NSString*) msg;

@end
