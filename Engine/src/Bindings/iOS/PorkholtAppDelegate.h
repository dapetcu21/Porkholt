/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <UIKit/UIKit.h>

@class PorkholtViewController;
class PHGameManager;

@interface PorkholtAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    PorkholtViewController *viewController;
    PHGameManager * gameManager;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet PorkholtViewController *viewController;

@end

