//
//  PHTouchInterface.m
//  Porkholt_iOS
//
//  Created by Marius Petcu on 2/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PHTouchInterface.h"
#import "PHAccelInterface.h"
#include "PHEventHandler.h"
#include "PHGameManager.h"
#import "PorkholtViewController.h"

PHTouchInterface * PHTouchInterfaceSingleton = NULL;

@implementation PHTouchInterface

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
		PHTouchInterfaceSingleton = self;
		self.multipleTouchEnabled = YES;
		[UIAccelerometer sharedAccelerometer].delegate=self;
 
		mutex = new PHMutex;
		queue.clear();
    }
    return self;
}

-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
	PHAcceleration accel;
	accel.x=[acceleration x];
	accel.y=[acceleration y];
	accel.z=[acceleration z];
	PHAccelInterface::setAcceleration(accel);
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code.
}
*/

-(void)processQueue
{
	mutex->lock();
	while (!queue.empty())
	{
		PHPoint pnt;
		TouchTask & task = queue.front();
		pnt.x = task.x;
		pnt.y = task.y;
		void * event = task.ud;
		switch (task.state)
		{
			case 0:
			{
				PHGameManagerSingleton->eventHandler()->touchDown(pnt, event);
				break;
			}
			case 1:
			{
                PHGameManagerSingleton->eventHandler()->touchUp(pnt, event);
				break;
			}
			case 2:
			{
                PHGameManagerSingleton->eventHandler()->touchCancelled(pnt, event);
				break;
			}
			case 3:
			{
                PHGameManagerSingleton->eventHandler()->touchMoved(pnt, event);
				break;
			}
		}
		queue.pop_front();
	}
	mutex->unlock();
}

-(void)addTask:(void *)ud state:(int)state X:(double)x Y:(double)y
{
	TouchTask tmp;
	tmp.ud = ud;
	tmp.x = x;
	tmp.y = y;
	tmp.state = state;
	mutex->lock();
	queue.push_back(tmp);
	mutex->unlock();
}

-(void)processEvent:(void*)event state:(int)state X:(double)x Y:(double)y
{
	PHPoint pnt;
	pnt.y = x*self.frame.size.height;
	pnt.x = y*self.frame.size.width;
	
	[self addTask:event state:state X:pnt.x Y:pnt.y];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		[self addTask:touch state:0 X:pnt.x Y:pnt.y];
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		[self addTask:touch state:3 X:pnt.x Y:pnt.y];
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		[self addTask:touch state:1 X:pnt.x Y:pnt.y];
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		[self addTask:touch state:2 X:pnt.x Y:pnt.y];
	}	
}

- (void)dealloc {
	mutex->release();
	queue.clear();
	[UIAccelerometer sharedAccelerometer].delegate=nil;
    [super dealloc];
}


@end
