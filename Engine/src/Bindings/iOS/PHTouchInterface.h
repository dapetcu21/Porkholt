/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <UIKit/UIKit.h>
#import <Porkholt/Core/PHMain.h>

struct TouchTask
{
	ph_float x,y;
	int state;
	void * ud;
};

class PHMutex;

@interface PHTouchInterface : UIView <UIAccelerometerDelegate> {
	PHMutex * mutex;
	list<TouchTask> q;
    ph_float scale;
}

extern PHTouchInterface * PHTouchInterfaceSingleton;

-(void)processQueue;
-(void)addTask:(void *)ud state:(int)state X:(ph_float)x Y:(ph_float)y;

-(void)processEvent:(void*)event state:(int)state X:(ph_float)x Y:(ph_float)y;

@end
