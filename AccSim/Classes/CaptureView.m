//
//  CaptureView.m
//  AccSim
//
//  Created by Marius Petcu on 3/23/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "CaptureView.h"
#import "TouchController.h"

@implementation CaptureView
@synthesize parent;

- (id)initWithFrame:(CGRect)frame {
    
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code.
    }
    return self;
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
		CGPoint rect = [touch locationInView:self];
		rect.x/=self.bounds.size.width;
		rect.y/=self.bounds.size.height;
		[parent processEvent:touch state:0 X:rect.x Y:rect.y];
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint rect = [touch locationInView:self];
		rect.x/=self.bounds.size.width;
		rect.y/=self.bounds.size.height;;
		[parent processEvent:touch state:1 X:rect.x Y:rect.y];
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{	
	for (UITouch * touch in touches)
	{	
		CGPoint rect = [touch locationInView:self];
		rect.x/=self.bounds.size.width;
		rect.y/=self.bounds.size.height;;
		[parent processEvent:touch state:3 X:rect.x Y:rect.y];
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch * touch in touches)
	{
		CGPoint rect = [touch locationInView:self];
		rect.x/=self.bounds.size.width;
		rect.y/=self.bounds.size.height;;
		[parent processEvent:touch state:2 X:rect.x Y:rect.y];
	}
}

- (void)dealloc {
    [super dealloc];
}


@end
