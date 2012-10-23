
/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "PHTouchInterface.h"
#import <Porkholt/Core/PHAccelInterface.h>
#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHMutex.h>
#import "PorkholtViewController.h"

PHTouchInterface * PHTouchInterfaceSingleton = NULL;

@implementation PHTouchInterface

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
		PHTouchInterfaceSingleton = self;
		self.multipleTouchEnabled = YES;
		[UIAccelerometer sharedAccelerometer].delegate=self;
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:1/30.0f];
        scale = ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)?[UIScreen mainScreen].scale:1.0f;
    }
    return self;
}

-(void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
	PHAcceleration accel;
	accel.x=-[acceleration y];
	accel.y=[acceleration x];
	accel.z=[acceleration z];
	PHAccelInterface::setAcceleration(accel);
}

- (PHPoint)adjustPoint:(CGPoint)p
{
    CGRect b = [self bounds];
    return PHPoint(
        p.x *  2.0 / b.size.width - 1,
        p.y * -2.0 / b.size.height + 1
        );
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
        PHGameManagerSingleton->eventHandler()->touchDown([self adjustPoint:[touch locationInView:self]], touch);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
        PHGameManagerSingleton->eventHandler()->touchMoved([self adjustPoint:[touch locationInView:self]], touch);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
        PHGameManagerSingleton->eventHandler()->touchUp([self adjustPoint:[touch locationInView:self]], touch);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
        PHGameManagerSingleton->eventHandler()->touchCancelled([self adjustPoint:[touch locationInView:self]], touch);
}

- (void)dealloc {
	[UIAccelerometer sharedAccelerometer].delegate=nil;
    [super dealloc];
}

@end

int PHEventHandler::modifierMask()
{
    return 0;
}
