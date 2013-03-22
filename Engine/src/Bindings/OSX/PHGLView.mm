/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Porkholt/Bindings/OSX/PHGLView.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#import <Porkholt/Core/PHGameManager.h>
#import <Porkholt/Core/PHEventHandler.h>
#import <Porkholt/Core/PHEvent.h>
#import <Porkholt/Core/PHWindowing.h>
#import <CoreVideo/CoreVideo.h>
#import <Porkholt/IO/PHDirectory.h>
#import <Porkholt/Core/PHTime.h>
#import <Porkholt/Core/PHAccelInterface.h>
#include <unimotion.h>

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
}

-(void)setVerticalSync:(BOOL)verticalSync
{
    if (verticalSync == vsync) return;
    GLint swapInt = verticalSync?1:0;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval]; 
    gameManager->setFpsCapped(verticalSync);
    vsync = verticalSync;
}

-(BOOL)verticalSync
{
    return vsync;
}

-(void)load
{
    if (gameManager) return;
    sms_type = detect_sms();
    
    timer = [NSTimer scheduledTimerWithTimeInterval:0.0f
                                                     target:self
                                                   selector:@selector(makeCurrentAndRender)
                                                   userInfo:nil
                                                    repeats:YES];

    NSRect frame = [self bounds];
    PHThread::mainThread();
    PHAutoreleasePool ap;
    gameManager = new PHGameManager;
    gameManager->setUserData(self);
    
    vsync = !(flags & PHWVSync);
    [self setVerticalSync:!vsync];
    
    CVDisplayLinkRef displayLink = nil;
	CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &displayLink);
	CVTime tmm = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(displayLink);
    double fps = round(double(tmm.timeScale)/tmm.timeValue);
	CVDisplayLinkRelease(displayLink);
    
    PHGameManagerInitParameters initParams;
    initParams.screenWidth = frame.size.width;
    initParams.screenHeight = frame.size.height;
    initParams.fps = fps;
    try { initParams.setResourceDirectory(PHInode::directoryAtFSPath(string([res UTF8String]) + "/rsrc")); }
    catch (const string & ex) {
        PHLog("Can't load resource directory: %s", ex.c_str());
    }
    initParams.entryPoint = entryPoint;
    PHWApplyFlags(gameManager, flags);
    [[self openGLContext] makeCurrentContext];
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

+(NSOpenGLPixelFormat *)pixelFormatWithFlags:(int)flags
{
    GLuint attributes[10];
    
    int n = 0;
    attributes[n++] = NSOpenGLPFADoubleBuffer;
    if (flags & PHWDepthBuffer)
    {
        attributes[n++] = NSOpenGLPFADepthSize;
        attributes[n++] = 32;
    }
    if (flags & PHWStencilBuffer)
    {
        attributes[n++] = NSOpenGLPFAStencilSize;
        attributes[n++] = 8;
    }
    attributes[n++] = 0;
    
    return [[[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*) attributes] autorelease];
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
    NSOpenGLPixelFormat * pf = [PHGLView pixelFormatWithFlags:_flags];
    self = [super initWithFrame:frameRect pixelFormat:pf];
    if (self)
    {
        res = [_res copy];
        entryPoint = _entryPoint;
        flags = _flags;
    }
    return self;
}

-(void)drawRect:(NSRect)dirty
{
    if (!gameManager)
    {
        [self load];
        [self render];
    }
}

-(BOOL) isOpaque {
    
    return YES;
}

-(PHGameManager*)gameManager
{
    return gameManager;
}

-(void)setGameManager:(PHGameManager*)gm
{
    gameManager = gm;
}

-(void)dealloc
{
    [self makeCurrent];
    [timer invalidate];
    gameManager->release();
    [res release];
    [super dealloc];
}

-(void)render
{
    double x,y,z;
    read_sms_real(sms_type, &x, &y, &z);
    static const double g = 9.81;
    PHAcceleration a = {.x = -x*g, .y = -z*g, .z = -y*g};
    PHAccelInterface::setAcceleration(a);
    gameManager->processInput();
    gameManager->renderFrame();
    glFlush();
    glSwapAPPLE();
}

-(void)reshape
{
    [super reshape];
    NSRect frame = [self bounds];
    if (manual)
        frame.size = msize;
    if (gameManager)
    {
        gameManager->setScreenSize(frame.size.width, frame.size.height);
        [self makeCurrent];
        [self render];
    }
}

-(void)update
{
    [super update];
}

-(NSPoint)adjustPoint:(NSPoint)p
{
    NSRect b = self.bounds;
    p.x = p.x *  2.0 / b.size.width - 1;
    p.y = p.y *  2.0 / b.size.height - 1;
    return p;
}

-(void)mouseDown:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchDown(PHPoint(p.x,p.y), (void*)1);
}

-(void)mouseDragged:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchMoved(PHPoint(p.x,p.y), (void*)1);
}

-(void)mouseUp:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchUp(PHPoint(p.x,p.y), (void*)1);
}

-(void)rightMouseDown:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchDown(PHPoint(p.x,p.y), (void*)2);
}

-(void)rightMouseDragged:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchMoved(PHPoint(p.x,p.y), (void*)2);
}

-(void)rightMouseUp:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchUp(PHPoint(p.x,p.y), (void*)2);
}

-(void)otherMouseDown:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchDown(PHPoint(p.x,p.y), (void*)([event buttonNumber]+1));
}

-(void)otherMouseDragged:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->touchMoved(PHPoint(p.x,p.y), (void*)([event buttonNumber]+1));
}

-(void)otherMouseUp:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
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
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->scrollWheel(PHPoint(p.x,p.y), PHPoint(dx,-dy), event);
}

-(void)magnifyWithEvent:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
    gameManager->eventHandler()->pinchZoom(PHPoint(p.x,p.y), [event magnification], event);
}

- (void)rotateWithEvent:(NSEvent *)event
{
    NSPoint p = [self adjustPoint:[self convertPoint:[event locationInWindow] fromView:nil]];
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

-(void)setManualSize:(NSSize)sz
{
    msize = sz;
    GLint dim[2] = {sz.width, sz.height};
    CGLContextObj ctx = (CGLContextObj)[self openGLContext].CGLContextObj;
    CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, dim);
    CGLEnable(ctx, kCGLCESurfaceBackingSize);
    manual = true;
    [self reshape];
}

-(void)setAutomaticSize
{
    if (!manual) return;
    manual = false;
    CGLContextObj ctx = (CGLContextObj)[self openGLContext].CGLContextObj;
    CGLDisable(ctx, kCGLCESurfaceBackingSize);
    [self reshape];
}

-(double)refreshRate
{
    return gameManager->framesPerSecond();
}

-(void)setRefreshRate:(double)refreshRate
{
    gameManager->setFramesPerSecond(refreshRate);
}

@end
