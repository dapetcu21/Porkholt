/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <AppKit/AppKit.h>

class PHGameManager;

@interface PHGLView : NSOpenGLView
{
    PHGameManager * gameManager;
    NSString * res;
    void (*entryPoint)(PHGameManager *);
    int flags;
    double time;
    double lastTime;
    bool vsync;
    bool manual;
    NSSize msize;
    NSTimer * timer;
}

-(id)initWithFrame:(NSRect)frameRect resourcePath:(NSString *)res entryPoint:(void (*)(PHGameManager *))entryPoint flags:(int)flags;
-(PHGameManager*)gameManager;
-(void)setManualSize:(NSSize)sz;
-(void)setAutomaticSize;


-(void)render;
-(void)render:(double)timeElapsed;
-(void)makeCurrent;
+(void)globalFrame;
+(void)globalFrame:(double)timeElapsed;
-(double)elapsedTime;


@property(nonatomic, assign) BOOL verticalSync;
@property(nonatomic, assign) double refreshRate;
@end
