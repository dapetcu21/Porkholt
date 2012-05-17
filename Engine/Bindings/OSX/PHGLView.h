//
//  PHGLView.h
//  Porkholt
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <AppKit/AppKit.h>

class PHGameManager;

@interface PHGLView : NSOpenGLView
{
    PHGameManager * gameManager;
    NSString * res;
    void (*entryPoint)(PHGameManager *);
    int flags;
    NSTimer * tm;
    double time;
    double lastTime;
    bool vsync;
    bool manual;
    NSSize msize;
}

-(id)initWithFrame:(NSRect)frameRect resourcePath:(NSString *)res entryPoint:(void (*)(PHGameManager *))entryPoint flags:(int)flags;
-(PHGameManager*)gameManager;
-(void)setManualSize:(NSSize)sz;
-(void)setAutomaticSize;


-(void)render;
-(void)render:(double)timeElapsed startTime:(double)start;
-(void)makeCurrent;
+(void)globalFrame;
+(void)globalFrame:(double)timeElapsed;
-(double)elapsedTime;


@property(nonatomic, assign) BOOL verticalSync;
@end
