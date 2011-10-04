//
//  OpenGLTimer.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreVideo/CoreVideo.h>

@class PLPorkholtView;

@interface OpenGLTimer : NSObject
{
    NSMutableSet * views;
    CVDisplayLinkRef displayLink;
}

-(void)addView:(PLPorkholtView*)v;
-(void)removeView:(PLPorkholtView*)v;
-(void)timerFired;

+(OpenGLTimer*)retainedInstance;
+(OpenGLTimer*)sharedInstance;

@end
