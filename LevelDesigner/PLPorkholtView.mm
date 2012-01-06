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
#import "OverlayController.h"
#import "InterfaceController.h"
#import "ObjectController.h"
#import "SubentityController.h"
#import "PLObject.h"
#import "SubentityViewController.h"

@interface NSEvent (PLDeviceDelta)
- (float)deviceDeltaX;
- (float)deviceDeltaY;
- (CGFloat)scrollingDeltaX;
- (CGFloat)scrollingDeltaY;
- (BOOL)hasPreciseScrollingDeltas;
@end

@implementation PLPorkholtView
@synthesize overlay;
@synthesize delegate;

-(void)resizeOverlay
{
    [overlay reshapeToRect:[self convertRectToBase:[self bounds]]];
}

-(void)loadWithPixelFormat:(NSOpenGLPixelFormat*)pf
{
    if (gameManager) return;
    
    [[OpenGLTimer retainedInstance] addView:self withPixelFormat:pf];
    
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

-(id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format
{
    self = [super initWithFrame:frameRect];
    if (self)
        [self loadWithPixelFormat:format];
    return self;
}

-(BOOL) isOpaque {
    
    return YES;
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
    [self resizeOverlay];
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
    [overlay release];
    [super dealloc];
}

-(void)render
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
    [self resizeOverlay];
    NSRect frame = [self bounds];
    if (gameManager)
        gameManager->setScreenSize(frame.size.width, frame.size.height);
}

-(void)update
{
    [super update];
    [self resizeOverlay];
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
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->scrollWheel(PHPoint(p.x,p.y), PHPoint(dx,-dy), event);
}

-(void)magnifyWithEvent:(NSEvent *)event
{
    if (rotating) return;
    zooming = true;
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    gameManager->eventHandler()->pinchZoom(PHPoint(p.x,p.y), [event magnification], event);
}

- (void)rotateWithEvent:(NSEvent *)event
{
    if (zooming) return;
    rotating = true;
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
    zooming = rotating = false;
}

-(IBAction)resetToOrigin:(id)sender
{
    ((PHScrollerView*)gameManager->rootView())->resetToOrigin();
}

void PHGameManager::entryPoint()
{
    PHScrollerView * v = new PHScrollerView;
    v->setScale(100,PHOriginPoint);
    view = v;
    [(PLPorkholtView*)(this->userData()) entryPoint];
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

-(id<PLPorkholtViewDelegate>)targetForForwardedMessage
{
    if ([delegate isKindOfClass:[InterfaceController class]])
    {
        ObjectController * oc = (ObjectController*)[(InterfaceController*)delegate model];
        if (!oc.objectMode) 
            return delegate;
        PLObject * sel = (PLObject*)[oc selectedEntity];
        if (![sel isKindOfClass:[PLObject class]]) 
            return delegate;
        MultipleListController * vc = [[sel subentityModel] controller];
        return (id<PLPorkholtViewDelegate>)vc;
    }
    return delegate;
}

-(void)new:(id)sender
{
    
    [[self targetForForwardedMessage] new:sender];
}

-(void)copy:(id)sender
{
    [[self targetForForwardedMessage] copy:sender];
}

-(void)paste:(id)sender
{
    [[self targetForForwardedMessage] paste:sender];
}

-(void)duplicate:(id)sender
{
    [[self targetForForwardedMessage] duplicate:sender];
}

-(void)delete:(id)sender
{
    [[self targetForForwardedMessage] delete:sender];
}

-(void)selectAll:(id)sender
{
    [[self targetForForwardedMessage] selectAll:sender];
}

-(BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
    return [[self targetForForwardedMessage] validateMenuItem:menuItem sentFrom:self];
}

-(IBAction)match:(id)sender
{
    [delegate match:sender];
}

-(IBAction)toggleShowMarkers:(id)sender
{
    [delegate toggleShowMarkers:sender];
}

-(IBAction)toggleShowImages:(id)sender
{
    [delegate toggleShowImages:sender];
}

-(IBAction)toggleShowFixtures:(id)sender
{
    [delegate toggleShowFixtures:sender];
}

-(IBAction)toggleShowJoints:(id)sender
{
    [delegate toggleShowJoints:sender];
}

-(IBAction)toggleObjectMode:(id)sender
{
    [delegate toggleObjectMode:sender];
}

-(IBAction)resetAspectRatio:(id)sender
{
    [delegate resetAspectRatio:sender];
}

+(void)globalFrame
{
    PHGameManager::globalFrame(1/60.0f);
}

@end
