#import "LPPHViewController.h"

#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHNormalImage.h>
#include <Porkholt/Core/PHGLTexture.h>
#include <Porkholt/IO/PHInode.h>
#import <QuartzCore/CADisplayLink.h>
#include <dlfcn.h>

static map<PHGameManager*, LPPHViewController*> gmToLP;

#define texoptions [NSDictionary dictionaryWithObjectsAndKeys: \
    [NSNumber numberWithBool:YES], GLKTextureLoaderOriginBottomLeft,\
    nil]

PHImage * LPPHGetWallpaperImage(PHGameManager * gm)
{
    map<PHGameManager*, LPPHViewController*>::iterator i = gmToLP.find(gm);
    if (i == gmToLP.end())
        return NULL;
    return i->second.wallpaperImage;
}

PHRect LPPHGetWallpaperBounds(PHGameManager * gm)
{
    map<PHGameManager*, LPPHViewController*>::iterator i = gmToLP.find(gm);
    if (i == gmToLP.end())
        return PHWholeRect;
    CGRect r = i->second.wallpaperRect;
    return PHRect(r.origin.x, r.origin.y, r.size.width, r.size.height);
}

#define LPGLKView PH_TOKENPASTE(LPGLKView, PH_APP_TARGET)

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
@synthesize wallpaperRect = _wallpaperRect;

-(void)_createDisplayLinkForScreen:(id)screen
{
    [super _createDisplayLinkForScreen:screen];
    [self.displayLink addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

-(id)initWithFlags:(int)flags entryPoint:(void(*)(PHGameManager*))ep userData:(void*)ud initDictionary:(NSDictionary*)init
{
    if ((self = [super init]))
    {
        _wallpaperRect = CGRectMake(0, 0, 1, 1);
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

    _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!_context)
        NSLog(@"Failed to create ES context");
        
    [EAGLContext setCurrentContext:_context];
    
    self.view = view;
    view.context = _context;
    view.delegate = self;
    

    [self setupGL];
}

-(void)dealloc
{
    [EAGLContext setCurrentContext:_context];
    [self tearDownGL];

    [_context release];
    [_bundle release];
    [_init release];
    if (_wallpaperImage)
        _wallpaperImage->release();

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
        PHDirectory * dir = PHInode::directoryAtFSPath(path);
        params.setResourceDirectory(dir);
    } catch (const string & ex) {
        PHLog("Can't load resource directory: %s", ex.c_str());
    }
    params.entryPoint = _entryPoint;
    PHWApplyFlags(gameManager, _flags | PHWVSync);
    if (params.screenWidth*params.screenWidth + params.screenHeight*params.screenHeight > 500000)
        gameManager->setPlatformSuffix(".hd");
    gameManager->setUserData(_ud);
    gameManager->init(params);
    _gm = gameManager;
    gmToLP.insert(make_pair(_gm, self));
}

-(void)tearDownGL
{
    if (_gm)
    {
        if (_wallpaperImage)
            _wallpaperImage->release();
        _wallpaperImage = NULL;
        gmToLP.erase(_gm);
        _gm->release();
    }
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
    [EAGLContext setCurrentContext:_context];
    if (_gm)
        _gm->setScreenSize(self.view.bounds.size.width, self.view.bounds.size.height);
}

-(UIImage*)screenShot
{
    [EAGLContext setCurrentContext:_context];
    return ((GLKView*)self.view).snapshot;
}

-(void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    [EAGLContext setCurrentContext:_context];
    [self render];
}

-(void)reloadPreferences
{
    [EAGLContext setCurrentContext:_context];
    [self resetIdleTimer];
    if (_gm)
        _gm->messageWithName("reloadPreferences")->broadcast(_gm);
}

-(void)setCurrentContext
{
    [EAGLContext setCurrentContext:_context];
}

#ifndef PH_LIVEPAPERS_FPS_STAGES
#define PH_LIVEPAPERS_FPS_STAGES {30, 30, 30, 15, 0}
#endif
static const NSInteger stages[] = PH_LIVEPAPERS_FPS_STAGES;

-(void)_setFPS:(unsigned int)stage
{
    [EAGLContext setCurrentContext:_context];
    if (stage >= (sizeof(stages)/sizeof(NSInteger)))
        stage  = (sizeof(stages)/sizeof(NSInteger)) - 1;
    if (stages[stage])
    {
        self.preferredFramesPerSecond = stages[stage];
        if (_gm)
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
    [EAGLContext setCurrentContext:_context];
    _showing = YES;
    if (_gm)
        _gm->appResumed();
    [self resetIdleTimer];
}

-(void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [EAGLContext setCurrentContext:_context];
    _showing = NO;
    [idleTimer invalidate];
    idleTimer = nil;
    if (_gm)
        _gm->appSuspended();
}

static float getIdleTimeout()
{
    static void * handle = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL);
    static float (*f)() = (float (*)())dlsym(handle, "LPGetIdleTimeout");
    if (f)
        return f();
    NSLog(@"Can't get idle timeout. Defaulting to 40 seconds"); 
    return 40.0f;
}

-(void)resetIdleTimer
{
    [idleTimer invalidate];
    idleTimer = [NSTimer scheduledTimerWithTimeInterval:getIdleTimeout()/((sizeof(stages)/sizeof(NSInteger)) - 1)
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
    [EAGLContext setCurrentContext:_context];
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchDown([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
    [EAGLContext setCurrentContext:_context];
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchMoved([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
    [EAGLContext setCurrentContext:_context];
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchUp([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gm) return;
    [EAGLContext setCurrentContext:_context];
	for (UITouch * touch in touches)
        _gm->eventHandler()->touchCancelled([self adjustPoint:[touch locationInView:self.view]], touch);
}

-(void)setWallpaperImage:(UIImage*)image
{
    [EAGLContext setCurrentContext:_context];
    if (_wallpaperImage)
        _wallpaperImage->release();
        
    if (_gm)
    {
        NSError * err;
        GLKTextureInfo * info = [GLKTextureLoader textureWithCGImage:image.CGImage options:texoptions error:&err];
        if (!info)
        {
            NSLog(@"Error occured while loading texture: %@", err);
            _wallpaperImage = NULL;
        } else {
            PHGLTexture2D * tex = new PHGLTexture2D(_gm, info.name, info.width, info.height);
            tex->setWrapS(PHGLTexture::clampToEdge);
            tex->setWrapT(PHGLTexture::clampToEdge);
            tex->setMinFilter(PHGLTexture::linear);
            tex->setMagFilter(PHGLTexture::linear);
            CGRect r = self.wallpaperRect;
            PHNormalImage * img = new PHNormalImage(tex, PHRect(r.origin.x, r.origin.y, r.size.width, r.size.height));
            tex->release();
            _wallpaperImage = img;
        }
        _gm->messageWithName("wallpaperImageChanged")->broadcast(_gm);
    } else {
        _wallpaperImage = NULL;
    }
}

-(PHNormalImage*)wallpaperImage
{
    return _wallpaperImage;
}

-(void)setWallpaperRect:(CGRect)r
{
    _wallpaperRect = r;
    if (_gm)
    {
        [EAGLContext setCurrentContext:_context];
        if (_wallpaperImage)
            _wallpaperImage->setTextureCoordinates(PHRect(r.origin.x, r.origin.y, r.size.width, r.size.height));
        _gm->messageWithName("wallpaperBoundsChanged")->broadcast(_gm);
    }
}

@end
