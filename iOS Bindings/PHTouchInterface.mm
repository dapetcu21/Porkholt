//
//  PHTouchInterface.m
//  Porkholt_iOS
//
//  Created by Marius Petcu on 2/18/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHTouchInterface.h"
#import "PHMain.h"
#import "PHAccelInterface.h"

@implementation PHTouchInterface

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
		self.multipleTouchEnabled = YES;
		[UIAccelerometer sharedAccelerometer].delegate=self;
		[UIAccelerometer sharedAccelerometer].updateInterval=1.0f/60;
        // Initialization code.
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

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		PHEventHandler::sharedInstance()->touchDown(PHMakePoint(pnt.x,pnt.y), touch);
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		PHEventHandler::sharedInstance()->touchMoved(PHMakePoint(pnt.x,pnt.y), touch);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		PHEventHandler::sharedInstance()->touchUp(PHMakePoint(pnt.x,pnt.y), touch);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint pnt = [touch locationInView:self];
		pnt.y = self.bounds.size.height - pnt.y;
		PHEventHandler::sharedInstance()->touchCancelled(PHMakePoint(pnt.x,pnt.y), touch);
	}	
}

- (void)dealloc {
    [super dealloc];
}


@end
