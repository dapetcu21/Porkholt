//
//  PHGLView.mm
//  Porkholt
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PHGLView.h"
#import "PHGameManager.h"
#import "PHEventHandler.h"
#import "PHEvent.h"
#import "OpenGLTimer.h"
#import "PHStartGame.h"


@interface NSEvent (PLDeviceDelta)
- (float)deviceDeltaX;
- (float)deviceDeltaY;
- (CGFloat)scrollingDeltaX;
- (CGFloat)scrollingDeltaY;
- (BOOL)hasPreciseScrollingDeltas;
@end

@implementation PHGLView

-(void)makeCurrentAndRender
{
    [self makeCurrent];
    double tim = [self elapsedTime];
    [PHGLView globalFrame:tim];
    [self render:tim startTime:time];
}

-(void)loadWithPixelFormat:(NSOpenGLPixelFormat*)pf
{
    if (gameManager) return;
    
    if (flags & PHStartGame_VSync)
        [[OpenGLTimer retainedInstance] addView:self withPixelFormat:pf];
    else
    {
        tm = [NSTimer scheduledTimerWithTimeInterval:0.0f 
                                                        target:self
                                                      selector:@selector(makeCurrentAndRender)
                                                      userInfo:nil
                                             repeats:YES];
    }
    
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = (flags & PHStartGame_VSync)?1:0;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
    
    NSRect frame = [self bounds];
    gameManager = new PHGameManager;
    gameManager->setUserData(self);
    PHGameManagerInitParameters initParams;
    initParams.screenWidth = frame.size.width;
    initParams.screenHeight = frame.size.height;
    initParams.fps = 60;
    initParams.resourcePath = string([res UTF8String]) + "/rsrc";
    initParams.entryPoint = entryPoint;
    if (flags & PHStartGame_Remote)
        gameManager->setUsesRemote(true);
    if (flags & PHStartGame_ShowFPS)
        gameManager->setShowsFPS(true);
    if (flags & PHStartGame_VSync)
        gameManager->setFpsCapped(true);
    gameManager->init(initParams);
}

+(NSOpenGLPixelFormat * )defaultPixelFormat
{
    static NSOpenGLPixelFormat * fmt = NULL;
    if (!fmt)
    {
        GLuint attributes[] = {
            NSOpenGLPFADoubleBuffer,
            NSOpenGLPFADepthSize, 32,
            0
        };
        
        fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*) attributes];
    }
    return fmt;
}
-(void)load
{
    [self loadWithPixelFormat:[PHGLView defaultPixelFormat]];
}


-(void)makeCurrent
{
    NSOpenGLContext* context = [self openGLContext];
    if ([context view] != self) {
        [context setView:self];
    }
    [context makeCurrentContext];
}

-(id)initWithFrame:(NSRect)frameRect resourcePath:(NSString *)_res entryPoint:(void (*)(PHGameManager *))_entryPoint flags:(int)_flags
{
    NSOpenGLPixelFormat * pf = [PHGLView defaultPixelFormat];
    self = [super initWithFrame:frameRect pixelFormat:pf];
    if (self)
    {
        res = [_res copy];
        entryPoint = _entryPoint;
        flags = _flags;
        [self loadWithPixelFormat:pf];
    }
    return self;
}

-(BOOL) isOpaque {
    
    return YES;
}

-(void)awakeFromNib
{
    [self load];
}

-(PHGameManager*)gameManager
{
    [self load];
    return gameManager;
}

-(void)dealloc
{
    gameManager->release();
    if (flags & PHStartGame_VSync)
    {
        [[OpenGLTimer sharedInstance] removeView:self];
        [[OpenGLTimer sharedInstance] release];
    }
    if (tm)
        [tm invalidate];
    [res release];
    [super dealloc];
}

-(double)elapsedTime
{
    double frameInterval = 1.0f/gameManager->framesPerSecond();
    if (flags & PHStartGame_frameAnimation)
        return frameInterval;
    lastTime = time;
    time = PHTime::getTime();
    double elapsedTime = (time-lastTime);
    if (flags & PHStartGame_VSync)
        return round(elapsedTime/frameInterval)*frameInterval;
    return elapsedTime;
}

-(void)render
{    
    double tim = [self elapsedTime];
    [self render:tim startTime:time];
}

-(void)render:(double)timeElapsed startTime:(double)start
{
    time = start;
    gameManager->setFrameBegin(start);
    gameManager->processInput();
    gameManager->renderFrame(timeElapsed);
    glFlush();
    glSwapAPPLE();
}

-(void)reshape
{
    [super reshape];
    NSRect frame = [self bounds];
    if (gameManager)
        gameManager->setScreenSize(frame.size.width, frame.size.height);
    [self makeCurrent];
    [self render];
}

-(void)update
{
    [super update];
}

-(void)mouseDown:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchDown(PHPoint(p.x,p.y), (void*)1);
}

-(void)mouseDragged:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchMoved(PHPoint(p.x,p.y), (void*)1);
}

-(void)mouseUp:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchUp(PHPoint(p.x,p.y), (void*)1);
}

-(void)rightMouseDown:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchDown(PHPoint(p.x,p.y), (void*)2);
}

-(void)rightMouseDragged:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchMoved(PHPoint(p.x,p.y), (void*)2);
}

-(void)rightMouseUp:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchUp(PHPoint(p.x,p.y), (void*)2);
}

-(void)otherMouseDown:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchDown(PHPoint(p.x,p.y), (void*)([event buttonNumber]+1));
}

-(void)otherMouseDragged:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchMoved(PHPoint(p.x,p.y), (void*)([event buttonNumber]+1));
}

-(void)otherMouseUp:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->touchUp(PHPoint(p.x,p.y), (void*)([event buttonNumber]+1));
}


- (void)scrollWheel:(NSEvent*)event
{
    double dx = event.deltaX * 10;
    double dy = event.deltaY * 10;
    if ([event respondsToSelector:@selector(hasPreciseScrollingDeltas)])
    {
        if ([event hasPreciseScrollingDeltas])
        {
            dx = [event scrollingDeltaX];
            dy = [event scrollingDeltaY];
        }
    } else
    if (CGEventGetIntegerValueField([event CGEvent ],kCGScrollWheelEventIsContinuous))
    {
        dx = [event deviceDeltaX];
        dy = [event deviceDeltaY];
    }
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->scrollWheel(PHPoint(p.x,p.y), PHPoint(dx,-dy), event);
}

-(void)magnifyWithEvent:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->pinchZoom(PHPoint(p.x,p.y), [event magnification], event);
}

- (void)rotateWithEvent:(NSEvent *)event
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->pinchRotate(PHPoint(p.x,p.y), -[event rotation], event);
}

- (void)beginGestureWithEvent:(NSEvent *)event
{
    gameManager->eventHandler()->multitouchBegin(event);
}

- (void)endGestureWithEvent:(NSEvent *)event
{
    gameManager->eventHandler()->multitouchEnd(event);
}

-(BOOL)acceptsFirstResponder
{
    return YES;
}

-(BOOL)becomeFirstResponder
{
    return YES;
}

-(BOOL)resignFirstResponder
{
    return YES;
}

int PHEventHandler::modifierMask()
{
    int mask = 0;
    NSUInteger nsmask = [NSEvent modifierFlags];
    if (nsmask & NSShiftKeyMask)
        mask |= shiftModifier;
    if (nsmask & NSAlternateKeyMask)
        mask |= optionModifier;
    if (nsmask & NSCommandKeyMask)
        mask |= commandModifier;
    if (nsmask & NSControlKeyMask)
        mask |= controlModifier;
    return mask;
}

+(void)globalFrame
{
    [PHGLView globalFrame:1.0f/60.0f];
}

+(void)globalFrame:(double)timeElapsed
{
    PHGameManager::globalFrame(timeElapsed);
}

@end
