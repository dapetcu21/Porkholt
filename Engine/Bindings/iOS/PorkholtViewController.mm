//
//  PorkholtViewController.m
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import <QuartzCore/QuartzCore.h>
#import <mach/mach.h>
#import <mach/mach_time.h>
#import "PorkholtViewController.h"
#import "PHGameManager.h"
#import "PHTouchInterface.h"
#import "PHMain.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#import "EAGLView.h"

PHGameManager * PHGameManagerSingleton;

#include "PHStartGame.h"

extern int PHStartGameFlags;
extern void (*PHStartGameEntryPoint)(PHGameManager *);
extern void * PHStartGameUD;

@interface PorkholtViewController ()
@property (nonatomic, retain) EAGLContext *context;
/*- (BOOL)loadShaders;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;*/
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
    if (PHStartGameFlags & PHStartGame_30FPS)
        return true;
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
    params.resourcePath = (string)([[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"rsrc"] UTF8String]);
    params.entryPoint = PHStartGameEntryPoint;
    gameManager->setUserData(PHStartGameUD);
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
    [PHTouchInterfaceSingleton processQueue];
    gameManager->processInput();
    [v setFramebuffer];
    
    ph_float elapsedTime;
    if (PHStartGameFlags & PHStartGame_frameAnimation)
        elapsedTime = 1.0f/gameManager->framesPerSecond();
    else
    {
        static ph_float time = 0;
        static ph_float lastTime = 0;
        
        ph_float frameInterval = 1.0f/gameManager->framesPerSecond();
        lastTime = time;
        time = PHTime::getTime();
        elapsedTime = time-lastTime;
        if (elapsedTime>1.5*frameInterval)
            elapsedTime = 1.5*frameInterval;
    }
    PHGameManager::globalFrame(elapsedTime);
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
