/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "PorkholtAppDelegate.h"
#import "PorkholtViewController.h"
#import <Porkholt/Core/PHGameManager.h>

@implementation PorkholtAppDelegate

@synthesize window;
@synthesize viewController;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    
    viewController = [[PorkholtViewController alloc] init];
    [window setRootViewController:viewController];
	[window layoutSubviews];
	[window makeKeyAndVisible];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [viewController stopAnimation];
    if (!gameManager)
        gameManager = [viewController gameManager];
    if (gameManager)
        gameManager->appSuspended();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [viewController startAnimation];
    if (!gameManager)
        gameManager = [viewController gameManager];
    if (gameManager)
        gameManager->appResumed();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [viewController stopAnimation];
    if (!gameManager)
        gameManager = [viewController gameManager];
    if (gameManager)
        gameManager->appQuits();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Handle any background procedures not related to animation here.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Handle any foreground procedures not related to animation here.
}

- (void)dealloc
{
    [viewController release];
    [window release];
    
    [super dealloc];
}

@end
