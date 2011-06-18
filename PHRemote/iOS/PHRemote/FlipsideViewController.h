//
//  FlipsideViewController.h
//  PHRemote
//
//  Created by Marius Petcu on 6/17/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <UIKit/UIKit.h>

@class MainViewController;

@interface FlipsideViewController : UIViewController<UITextFieldDelegate> {
    IBOutlet UITextField * hostfield,*portfield,*freqfield;
    IBOutlet UISwitch * accswitch,*touchswitch,*groupswitch;
    MainViewController * delegate;
}

@property (nonatomic, assign) MainViewController * delegate;

- (IBAction)done:(id)sender;
- (IBAction)settingsChanged:(id)sender;

@end


@protocol FlipsideViewControllerDelegate
- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
@end
