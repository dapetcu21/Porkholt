#import "LPPHViewController.h"

#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/IO/PHInode.h>

@interface GLKViewController(_private)
-(CADisplayLink*)displayLink;
-(void)_createDisplayLinkForScreen:(id)screen;
@end

@interface LPGLKView : GLKView
@end
@implementation LPGLKView
-(void)setFrame:(CGRect)bounds
{
    [super setFrame:bounds];
    [(LPPHViewController*)self.delegate reshape];
}
@end

@implementation LPPHViewController
@synthesize context = _context;
@synthesize bundle = _bundle;

-(id)initWithFlags:(int)flags entryPoint:(void(*)(PHGameManager*))ep userData:(void*)ud initDictionary:(NSDictionary*)init
{
    if ((self = [super init]))
    {
        _entryPoint = ep;
        _ud = ud;
        _flags = flags;
        _init = [init retain];
    }
    return self;
}

-(void)loadView
{
    if (!_bundle)
        _bundle = [[NSBundle mainBundle] retain];
    GLKView * view = [[LPGLKView alloc] initWithFrame:[UIScreen mainScreen].bounds];
    view.multipleTouchEnabled = YES;

    _context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];

    if (!_context) {
        NSLog(@"Failed to create ES context");
    }
    
    self.view = view;
    view.context = _context;
    view.delegate = self;
    
    [EAGLContext setCurrentContext:_context];

    [self setupGL];
}

-(void)dealloc
{
    [EAGLContext setCurrentContext:_context];
    [self tearDownGL];

    [_context release];
    [_bundle release];
    [_init release];

    [super dealloc];
}

-(void)setupGL
{
    PHAutoreleasePool ap;
    PHGameManager * gameManager = new PHGameManager;
    
    PHGameManagerInitParameters params;
    UIScreen * s = [UIScreen mainScreen];
    ph_float scale = ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)?s.scale:1.0f;
    params.screenWidth = self.view.bounds.size.width*scale;
    params.screenHeight = self.view.bounds.size.height*scale;
    params.fps = self.framesPerSecond;
    params.dpi = 160*scale;
    try {
        string path = [[[_init objectForKey:@"LCInitPluginPath"] stringByReplacingOccurrencesOfString:@".dylib" withString:@"-rsrc"] UTF8String];
        PHLog("File path: %s", path.c_str());
        PHDirectory * dir = PHInode::directoryAtFSPath(path);
        params.setResourceDirectory(dir);
    } catch (const string & ex) {
        PHLog("Can't load resource directory: %s", ex.c_str());
    }
    params.entryPoint = _entryPoint;
    if (_flags & PHWShowFPS)
        gameManager->setShowsFPS(true);
    if (_flags & PHWRemote)
        gameManager->setUsesRemote(true);
    if (params.screenWidth*params.screenWidth + params.screenHeight*params.screenHeight > 500000)
        gameManager->setPlatformSuffix(".hd");
    if (_flags & PHWFrameAnimation)
        gameManager->setFrameAnimation(true);
    gameManager->setFpsCapped(true);
    gameManager->setUserData(_ud);
    NSLog(@"hamham");
    gameManager->init(params);
    _gm = gameManager;
}

-(void)tearDownGL
{
    if (_gm)
        _gm->release();
    _gm = NULL;
}

-(void)update
{
}

-(void)render
{
    if (_gm)
    {
        _gm->processInput();
        _gm->renderFrame();
    }
}

-(void)reshape
{
    if (_gm)
        _gm->setScreenSize(self.view.bounds.size.width, self.view.bounds.size.height);
}

-(UIImage*)screenShot
{
    return ((GLKView*)self.view).snapshot;
}

-(void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    [self render];
}


-(void)setCurrentContext
{
    [EAGLContext setCurrentContext:_context];
}

-(void)_setFPS:(unsigned int)stage
{
    const NSInteger stages[] = {60, 30, 15, 15, 0};
    if (stage >= (sizeof(stages)/sizeof(NSInteger)))
        stage  = (sizeof(stages)/sizeof(NSInteger)) - 1;
    if (stages[stage])
    {
        self.preferredFramesPerSecond = stages[stage];
        _gm->setFramesPerSecond(self.framesPerSecond);
        self.paused = !_showing;
    } else {
        self.paused = YES;
    }
}

-(void)timerFired
{
    [self _setFPS:++fpsStage];
}

-(void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    _showing = YES;
    if (_gm)
        _gm->appResumed();
    [self resetIdleTimer];
}

-(void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    _showing = NO;
    [idleTimer invalidate];
    idleTimer = nil;
    if (_gm)
        _gm->appSuspended();
}

-(void)resetIdleTimer
{
    [idleTimer invalidate];
    idleTimer = [NSTimer scheduledTimerWithTimeInterval:10.0f
        target:self
        selector:@selector(timerFired)
        userInfo:nil
        repeats:YES];
    [self _setFPS:(fpsStage = 0)];
}

-(PHPoint)adjustPoint:(CGPoint)p
{
    CGRect b = [self.view bounds];
    return PHPoint(
        p.x *  2.0 / b.size.width - 1,
        p.y * -2.0 / b.size.height + 1
        );
}

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchDown([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchMoved([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchUp([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchCancelled([self adjustPoint:[touch locationInView:self.view]], touch);
}

@end
