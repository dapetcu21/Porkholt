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
    int sms_type;
}

-(id)initWithFrame:(NSRect)frameRect resourcePath:(NSString *)res entryPoint:(void (*)(PHGameManager *))entryPoint flags:(int)flags;

-(void)setManualSize:(NSSize)sz;
-(void)setAutomaticSize;

-(void)render;
-(void)makeCurrent;
-(void)makeCurrentAndRender;


@property(nonatomic, assign) BOOL verticalSync;
@property(nonatomic, assign) double refreshRate;
@property(nonatomic, assign) PHGameManager * gameManager;
@end
