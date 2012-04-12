//
//  OpenGLTimer.mm
//  Porkholt
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "OpenGLTimer.h"
#import "PHGLView.h"
#import "PHTime.h"
#import <CoreVideo/CoreVideo.h>

static OpenGLTimer * OpenGLTimer_singleton = nil;


static CVReturn MyDisplayLinkCallback (
                                CVDisplayLinkRef displayLink,
                                const CVTimeStamp *inNow,
                                const CVTimeStamp *inOutputTime,
                                CVOptionFlags flagsIn,
                                CVOptionFlags *flagsOut,
                                void *displayLinkContext
                                )
{
    [[OpenGLTimer sharedInstance] performSelectorOnMainThread:@selector(timerFired) withObject:nil waitUntilDone:YES];
    return kCVReturnSuccess;
}


@implementation OpenGLTimer

-(void)timerFired
{
    lastTime = time;
    time = PHTime::getTime();
    static const double frameInterval = 1.0f/60.0f;
    double elapsedTime = round((time-lastTime)/frameInterval)*frameInterval;
    [PHGLView globalFrame:elapsedTime];
    for (PHGLView * v in views)
    {
        [v makeCurrent];
        [v render:elapsedTime startTime:time];
    }
}

- (id)init
{
    self = [super init];
    if (self) {
        views = [[NSMutableSet alloc] init];
        CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
        CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
        CVDisplayLinkSetCurrentCGDisplay(displayLink,0);
        CVDisplayLinkStart(displayLink);
    }
    
    return self;
}

-(void)dealloc
{
    CVDisplayLinkRelease(displayLink);
    [views release];
    [context release];
    OpenGLTimer_singleton = nil;
    [super dealloc];
}

-(void)addView:(PHGLView*)v withPixelFormat:(NSOpenGLPixelFormat*)pf
{
    NSOpenGLContext * c = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:context];
    if (!context)
        context = [c retain];
    [v setOpenGLContext: c];
    [c release];
    [views addObject:v];
}
-(BOOL)removeView:(PHGLView*)v
{
    if (![views containsObject:v]) return NO;
    [views removeObject:v];
    return YES;
}

+(OpenGLTimer*)retainedInstance
{
    if (!OpenGLTimer_singleton)
        OpenGLTimer_singleton = [[OpenGLTimer alloc] init];
    else
        [OpenGLTimer_singleton retain];
    return OpenGLTimer_singleton;
}

+(OpenGLTimer*)sharedInstance
{
    return OpenGLTimer_singleton;
}

@end
