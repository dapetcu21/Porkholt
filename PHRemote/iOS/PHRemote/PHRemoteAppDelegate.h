//
//  PHRemoteAppDelegate.h
//  PHRemote
//
//  Created by Marius Petcu on 6/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MainViewController;

@interface PHRemoteAppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet MainViewController *mainViewController;

@end
