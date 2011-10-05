//
//  PLPorkholtView.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLPorkholtView.h"
#import "PHGameManager.h"

#import "PHTestViewController.h"
#import "PHNavigationController.h"
#import "OpenGLTimer.h"
#import "PHScrollerView.h"
#import "PHEventHandler.h"

@interface NSEvent (PLDeviceDelta)
- (float)deviceDeltaX;
- (float)deviceDeltaY;
- (CGFloat)scrollingDeltaX;
- (CGFloat)scrollingDeltaY;
- (BOOL)hasPreciseScrollingDeltas;
@end

@implementation PLPorkholtView

-(void)load
{
    if (gameManager) return;
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
    
    NSRect frame = [self bounds];
    gameManager = new PHGameManager;
    gameManager->setUserData(self);
    gameManager->init(frame.size.width, frame.size.height, 60);
    
    [[OpenGLTimer retainedInstance] addView:self];
}

-(id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format
{
    self = [super initWithFrame:frameRect pixelFormat:format];
    if (self)
        [self load];
    return self;
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self)
        [self load];
    return self;
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

-(PHView*)worldView
{
    return worldView;
}

-(void)setNeedsDisplayYes
{
    [self setNeedsDisplay:YES];
}

-(void)dealloc
{
    gameManager->release();
    [[OpenGLTimer sharedInstance] removeView:self];
    [[OpenGLTimer sharedInstance] release];
    [super dealloc];
}

-(void)drawRect:(NSRect)dirtyRect
{
    gameManager->processInput();
    
    static double time = 0;
    static double lastTime = 0;
    
    double frameInterval = 1.0f/gameManager->framesPerSecond();
    lastTime = time;
    time = PHTime::getTime();
    double elapsedTime = time-lastTime;
    if (elapsedTime>1.5*frameInterval)
        elapsedTime = 1.5*frameInterval;
    gameManager->renderFrame(elapsedTime);
    glFlush();
}

-(void)reshape
{
    [super reshape];
    NSRect frame = [self bounds];
    if (gameManager)
        gameManager->setScreenSize(frame.size.width, frame.size.height);
}

-(void)entryPoint
{
    worldView = new PHView(PHWholeRect);
    ((PHScrollerView*)gameManager->rootView())->setContentView(worldView);
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
    PHLog("%lf %lf",dx,dy);
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
    gameManager->eventHandler()->pinchRotate(PHPoint(p.x,p.y), [event rotation], event);
}

- (void)beginGestureWithEvent:(NSEvent *)event
{
    gameManager->eventHandler()->multitouchBegin(event);
}

- (void)endGestureWithEvent:(NSEvent *)event
{
    gameManager->eventHandler()->multitouchEnd(event);
}

void PHGameManager::entryPoint()
{
    PHScrollerView * v = new PHScrollerView;
    v->setScale(50,PHOriginPoint);
    view = v;
    [(PLPorkholtView*)(this->userData()) entryPoint];
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

@end
