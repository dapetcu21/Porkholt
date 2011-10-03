//
//  OpenGLTimer.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "OpenGLTimer.h"
#import "PLPorkholtView.h"

static OpenGLTimer * OpenGLTimer_singleton = nil;

@implementation OpenGLTimer

-(void)timerFired
{
    for (PLPorkholtView * v in views)
    {
        [[v openGLContext] makeCurrentContext];
        [v drawRect:[v bounds]];
    }
}

- (id)init
{
    self = [super init];
    if (self) {
        views = [[NSMutableSet alloc] init];
        timer = [NSTimer scheduledTimerWithTimeInterval:1.0f/60.0f target:self selector:@selector(timerFired) userInfo:nil repeats:YES];
    }
    
    return self;
}

-(void)dealloc
{
    [timer invalidate];
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
