//
//  OpenGLTimer.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
    [[OpenGLTimer sharedInstance] timerFired];
    return kCVReturnSuccess;
}


@implementation OpenGLTimer

-(void)timerFired
{
    for (PLPorkholtView * v in views)
    {
        [[v openGLContext] makeCurrentContext];
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
    OpenGLTimer_singleton = nil;
    [super dealloc];
}

-(void)addView:(PLPorkholtView*)v
{
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
