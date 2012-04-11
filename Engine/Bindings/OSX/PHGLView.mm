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
    [self render];
    [PHGLView globalFrame];
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
    
    GLint swapInt = 1;
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
    gameManager->init(initParams);
}

-(void)load
{
    [self loadWithPixelFormat:[NSOpenGLView defaultPixelFormat]];
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
    self = [super initWithFrame:frameRect pixelFormat:[NSOpenGLView defaultPixelFormat]];
    if (self)
    {
        res = [_res copy];
        entryPoint = _entryPoint;
        flags = _flags;
        [self load];
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

-(void)render
{    
    gameManager->processInput();
    
    static double time = 0;
    static double lastTime = 0;
    
    double frameInterval = 1.0f/gameManager->framesPerSecond();
    if (flags & PHStartGame_frameAnimation)
        gameManager->renderFrame(frameInterval);
    else
    {
        lastTime = time;
        time = PHTime::getTime();
        double elapsedTime = (time-lastTime);
        if (flags & PHStartGame_VSync)
            elapsedTime = round(elapsedTime/frameInterval)*frameInterval;
        gameManager->renderFrame(elapsedTime);
    }
    glFlush();
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
    PHGameManager::globalFrame(1/60.0f);
}

@end
