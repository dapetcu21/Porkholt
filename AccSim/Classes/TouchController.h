//
//  TouchController.h
//  AccSim
//
//  Created by Marius Petcu on 3/23/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <UIKit/UIKit.h>

@class NetworkView;
@interface TouchController : UIViewController {
	NetworkView * parent;
}

@property(nonatomic,assign) NetworkView * parent;

-(void)processEvent:(UITouch*)touch state:(int)state X:(double)x Y:(double)y;

@end
