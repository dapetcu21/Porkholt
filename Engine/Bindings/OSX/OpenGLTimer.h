//
//  OpenGLTimer.h
//  Porkholt
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <CoreVideo/CoreVideo.h>

@class PHGLView;

@interface OpenGLTimer : NSObject
{
    NSMutableSet * views;
    CVDisplayLinkRef displayLink;
    NSOpenGLContext * context;
}

-(void)addView:(PHGLView*)v withPixelFormat:(NSOpenGLPixelFormat*)pf;
-(void)removeView:(PHGLView*)v;
-(void)timerFired;

+(OpenGLTimer*)retainedInstance;
+(OpenGLTimer*)sharedInstance;

@end
