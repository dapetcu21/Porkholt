/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "OpenGLTimer.h"
#import "PLPorkholtView.h"
#import "PHScrollerView.h"
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
    [PLPorkholtView globalFrame];
    for (PLPorkholtView * v in views)
    {
        [v makeCurrent];
        [v render];
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

-(void)addView:(PLPorkholtView*)v withPixelFormat:(NSOpenGLPixelFormat*)pf
{
    NSOpenGLContext * c = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:context];
    if (!context)
        context = [c retain];
    [v setOpenGLContext: c];
    [c release];
    [views addObject:v];
}
-(void)removeView:(PLPorkholtView*)v
{
    [views removeObject:v];
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
