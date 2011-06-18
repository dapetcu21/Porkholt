//
//  MainViewController.h
//  PHRemote
//
//  Created by Marius Petcu on 6/17/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "URClient.h"
#include <list>

@class FlipsideViewController;

struct touch
{
    void * id;
    double x,y;
    int state;
};

@interface MainViewController : UIViewController <UIAccelerometerDelegate> {
    URClient remote;
    NSString * host, * port;
    IBOutlet UILabel * status;
    BOOL sendAcc,sendTouch,groupPackets;
    
    std::list<touch> queue;
}


- (void)start;
- (void)stop;

@property(nonatomic,retain) NSString * host;
@property(nonatomic,retain) NSString * port;
@property(nonatomic,assign) BOOL sendAcc;
@property(nonatomic,assign) BOOL sendTouch;
@property(nonatomic,assign) BOOL groupPackets;

- (void)flipsideViewControllerDidFinish:(FlipsideViewController *)controller;
- (void)sendTouchPack:(int)phase withTouch:(UITouch*)touch inView:(UIView*)view;

@end
