//
//  PHTouchInterface.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 2/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PHTouchInterface : UIView <UIAccelerometerDelegate> {

}

extern PHTouchInterface * PHTouchInterfaceSingleton;

-(void)processEvent:(void*)event state:(int)state X:(double)x Y:(double)y;

@end
