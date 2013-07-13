/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <QuartzCore/QuartzCore.h>
#import <mach/mach.h>
#import <mach/mach_time.h>
#import "PorkholtViewController.h"
#include <Porkholt/Core/PHGameManager.h>
#import "PHTouchInterface.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#import "EAGLView.h"

PHGameManager * PHGameManagerSingleton;

#include <Porkholt/Core/PHWindowing.h>

extern int PHWFlags;
extern void (*PHWEntryPoint)(PHGameManager *);
extern void * PHWUD;

@implementation PorkholtViewController

@synthesize animating;

- (NSString *) platform
{
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	char *machine = (char*)malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString *platform = [NSString stringWithCString:machine encoding:NSUTF8StringEncoding];
	free(machine);
	return platform;
}

- (bool) dumbDevice
{
#ifdef PH_SIMULATOR
    return true;
#endif
	NSString * platform = [self platform];
	PHLog("Running on %s",[platform UTF8String]);
	if ([platform isEqual:@"iPhone1,1"]|| //iPhone 1G
		[platform isEqual:@"iPhone1,2"]|| //iPhone 3G
		[platform isEqual:@"iPod1,1"]||   //iPod Touch 1G
		[platform isEqual:@"iPod2,1"])    //iPod Touch 2G
		return true;
	return false;
}

- (void)loadView
{
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    CGRect bounds = [UIScreen mainScreen].bounds;
    if (orientation == UIInterfaceOrientationLandscapeLeft || orientation == UIInterfaceOrientationLandscapeRight)
    {
        CGFloat w = bounds.size.width;
        bounds.size.width = bounds.size.height;
        bounds.size.height = w;
    }
	EAGLView * view = [[EAGLView alloc] initWithFrame:bounds];
    view.delegate = (id<EAGLViewDelegate>)self;
	
    PHTouchInterface * touchView = [[PHTouchInterface alloc] initWithFrame:view.bounds];
	touchView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
	[view addSubview:touchView];
	[touchView release];
	
	self.view = view;
	
	animating = FALSE;
    
    v = view;
    fps = 60;
	if ([self dumbDevice])
		fps = 30;

    [self initPorkholt];
}

- (void)dealloc
{
	[super dealloc];
}

- (void)initPorkholt
{
    [v setFramebuffer];
    NSRunLoop *theRL = [NSRunLoop currentRunLoop];
    
    float FPS = fps;

    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(openGLFrame:)];
    displayLink.frameInterval = round(60.0f/fps);
    FPS = 1.0f/(1.0f/60*displayLink.frameInterval);
    displayLink.paused = !animating;
    [displayLink addToRunLoop:theRL forMode:NSDefaultRunLoopMode];
    
    PHAutoreleasePool ap;
    gameManager = new PHGameManager;
    
    UIScreen * s = [UIScreen mainScreen];
    float scale = ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)?s.scale:1.0f;
    CGSize sz = ((EAGLView*)self.view).framebufferSize;

    PHGameManagerInitParameters params;
    params.screenWidth = sz.width;
    params.screenHeight = sz.height;
    params.fps = FPS;
    params.dpi = 160*scale;
    try {
        PHDirectory * dir = PHInode::directoryAtFSPath((string)([[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"rsrc"] UTF8String]));
        params.setResourceDirectory(dir);
    } catch (const string & ex) {
        PHLog("Can't load resource directory: %s", ex.c_str());
    }
    params.entryPoint = PHWEntryPoint;
    PHWApplyFlags(gameManager, PHWFlags | PHWVSync);
    if (params.screenWidth*params.screenWidth + params.screenHeight*params.screenHeight > 500000)
        gameManager->setPlatformSuffix(".hd");
    gameManager->setUserData(PHWUD);
    gameManager->init(params);
    PHGameManagerSingleton = gameManager;
}

- (void)openGLFrame:(CADisplayLink*)displayLink
{
    [v setFramebuffer];
    gameManager->renderFrame();
    
    if (![v presentFramebuffer])
        PHLog("ERROR: Couldn't swap buffers");
}

-(void)eaGLViewCreatedFramebuffer:(EAGLView*)view
{
    NSLog(@"createdFramebuffer");
    if (!gameManager) return;
    CGSize sz = view.framebufferSize; 
    gameManager->setScreenSize(sz.width, sz.height);
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [self stopAnimation];
    [super viewDidDisappear:animated];
}

- (void)startAnimation
{
    animating = TRUE;
    displayLink.paused = !animating;
}

- (void)stopAnimation
{
    animating = FALSE;
    displayLink.paused = !animating;
}

- (void)didReceiveMemoryWarning
{
    gameManager->memoryWarning();
    [super didReceiveMemoryWarning];
}

-(PHGameManager*)gameManager
{
    return gameManager;
}


@end
