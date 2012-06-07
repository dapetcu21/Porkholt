#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h> 
#import <AVFoundation/AVFoundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#endif

typedef void (^FILogger)(NSString *fmt, ...);

extern const FILogger FILoggerNull;
extern const FILogger FILoggerNSLog;
