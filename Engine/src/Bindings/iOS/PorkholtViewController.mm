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
#import "EAGLView.h"

PHGameManager * PHGameManagerSingleton;

#include <Porkholt/Core/PHWindowing.h>

extern int PHWFlags;
extern void (*PHWEntryPoint)(PHGameManager *);
extern void * PHWUD;

@interface PorkholtViewController ()
@property (nonatomic, retain) EAGLContext *context;
@end 

@implementation PorkholtViewController

@synthesize animating, context;

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
	EAGLView * view = [[EAGLView alloc] initWithFrame:[UIScreen mainScreen].bounds];
	
    PHTouchInterface * touchView = [[PHTouchInterface alloc] initWithFrame:view.bounds];
	touchView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
	[view addSubview:touchView];
	[touchView release];
	
	self.view = view;
	
	animating = FALSE;
    
    thread = [[NSThread alloc] initWithTarget:self
                                     selector:@selector(openGLThread) 
                                       object:nil];
    exitCondition = [[NSConditionLock alloc] init];
    v = view;
    fps = 60;
	if ([self dumbDevice])
		fps = 30;
    [thread start];
}

- (void)dealloc
{
	[thread cancel];
    [exitCondition lockWhenCondition: 1];
    [exitCondition unlock];
    [thread release];
    [exitCondition release];
	[super dealloc];
}

- (void)dummy
{
    
}

- (void)openGLThread
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    [v initMain];
    [v setFramebuffer];
    NSRunLoop *theRL = [NSRunLoop currentRunLoop];
    
    ph_float FPS = fps;
#ifdef PH_SIMULATOR
    BOOL useDisplayLink = NO;
#else
    BOOL useDisplayLink = [[[UIDevice currentDevice] systemVersion] compare:@"3.1" options:NSNumericSearch] != NSOrderedAscending;
#endif

    if (useDisplayLink)
    {
        CADisplayLink * displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(openGLFrame:)];
        displayLink.frameInterval = round(60.0f/fps);
        FPS = 1.0f/(1.0f/60*displayLink.frameInterval);
        displayLink.paused = YES;
        dl = displayLink;
        [displayLink addToRunLoop:theRL forMode:NSDefaultRunLoopMode];
    } else {
        [NSTimer scheduledTimerWithTimeInterval:1024 target:self selector:@selector(dummy) userInfo:nil repeats:YES]; //to keep the run loop blocking
    }
    
    gameManager = new PHGameManager;
    
    PHGameManagerInitParameters params;
    UIScreen * s = [UIScreen mainScreen];
    ph_float scale = ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)?s.scale:1.0f;
    params.screenHeight = s.bounds.size.width*scale;
    params.screenWidth = s.bounds.size.height*scale;
    params.fps = FPS;
    params.dpi = 160*scale;
    PHDirectory * dir = PHInode::directoryAtFSPath((string)([[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"rsrc"] UTF8String]));
    params.setResourceDirectory(dir);
    dir->release();
    params.entryPoint = PHWEntryPoint;
    if (PHWFlags & PHWShowFPS)
        gameManager->setShowsFPS(true);
    if (PHWFlags & PHWRemote)
        gameManager->setUsesRemote(true);
    if (params.screenWidth*params.screenWidth + params.screenHeight*params.screenHeight > 500000)
        gameManager->setPlatformSuffix(".hd");
    gameManager->setFpsCapped(true);
    gameManager->setUserData(PHWUD);
    gameManager->init(params);
    PHGameManagerSingleton = gameManager;
    
    while (![thread isCancelled] && [theRL runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]]);
    
    [exitCondition lock];
    [exitCondition unlockWithCondition: 1];
    [pool drain];
    [dl release];
}

- (void)openGLFrame:(CADisplayLink*)displayLink
{
    gameManager->processInput();
    [v setFramebuffer];
    
    ph_float elapsedTime;
    if (PHWFlags & PHWFrameAnimation)
    {
        elapsedTime = 1.0f/gameManager->framesPerSecond();
    }
    else
    {
        static ph_float time = 0;
        static ph_float lastTime;
        
        ph_float frameInterval = 1.0f/gameManager->framesPerSecond();
        lastTime = time;
        time = PHTime::getTime();
        if (lastTime == 0)
            lastTime = time - frameInterval;
        elapsedTime = time-lastTime;
        elapsedTime = round(elapsedTime/frameInterval)*frameInterval;
    }
    gameManager->renderFrame(elapsedTime);
    
    if (![v presentFramebuffer])
        PHLog("ERROR: Couldn't swap buffers");
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    [super viewWillDisappear:animated];
}

- (void)startAnimation
{
    if (!animating)
    {
        animating = TRUE;
        [self performSelector:@selector(defferedSetPaused:) onThread:thread withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
    }
}

- (void)stopAnimation
{
    if (animating)
    {
        animating = FALSE;
        [self performSelector:@selector(defferedSetPaused:) onThread:thread withObject:[NSNumber numberWithBool:YES] waitUntilDone:NO];
    }
}

- (void)defferedSetPaused:(NSNumber*)val
{
    if (dl)
        dl.paused = [val boolValue];
    else
    {
        if ([val boolValue])
            [timer invalidate];
        else
            timer = [NSTimer scheduledTimerWithTimeInterval:(1.0f/fps) target:self selector:@selector(openGLFrame:) userInfo:nil repeats:YES];
    }
}

- (void)didReceiveMemoryWarning
{
    gameManager->memoryWarning();
    [super didReceiveMemoryWarning];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}


-(PHGameManager*)gameManager
{
    return gameManager;
}


@end
