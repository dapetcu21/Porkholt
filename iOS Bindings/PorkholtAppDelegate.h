//
//  PorkholtAppDelegate.h
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Home. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PorkholtViewController;

@interface PorkholtAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    PorkholtViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet PorkholtViewController *viewController;

@end

