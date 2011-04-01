//
//  PHTouchInterface.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 2/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PHMain.h"

#include <list>

struct TouchTask
{
	double x,y;
	int state;
	void * ud;
};

class PHMutex;

@interface PHTouchInterface : UIView <UIAccelerometerDelegate> {
	PHMutex * mutex;
	list<TouchTask> queue;
}

extern PHTouchInterface * PHTouchInterfaceSingleton;

-(void)processQueue;
-(void)addTask:(void *)ud state:(int)state X:(double)x Y:(double)y;

-(void)processEvent:(void*)event state:(int)state X:(double)x Y:(double)y;

@end
