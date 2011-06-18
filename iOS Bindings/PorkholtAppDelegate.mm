//
//  PorkholtAppDelegate.m
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import "PorkholtAppDelegate.h"
#import "PorkholtViewController.h"
#import "PHMainEvents.h"

@implementation PorkholtAppDelegate

@synthesize window;
@synthesize viewController;

- (void)applicationDidFinishLaunching:(UIApplication *)application
{
	window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
	viewController = [[PorkholtViewController alloc] init];

	[window addSubview:viewController.view];
	[window layoutSubviews];
	[window makeKeyAndVisible];
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [viewController stopAnimation];
	PHMainEvents::sharedInstance()->appSuspended();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [viewController startAnimation];
	PHMainEvents::sharedInstance()->appResumed();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [viewController stopAnimation];
	PHMainEvents::sharedInstance()->appQuits();
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
