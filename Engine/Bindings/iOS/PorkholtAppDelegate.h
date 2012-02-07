//
//  PorkholtAppDelegate.h
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

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

