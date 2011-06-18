//
//  CaptureView.m
//  PHRemote
//
//  Created by Marius Petcu on 6/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "CaptureView.h"
#import "MainViewController.h"


@implementation CaptureView
@synthesize delegate;

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}
*/

- (void)dealloc
{
    [super dealloc];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        [delegate sendTouchPack:0 withTouch:touch inView:self];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        [delegate sendTouchPack:1 withTouch:touch inView:self];
    }
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        [delegate sendTouchPack:2 withTouch:touch inView:self];
    }
}
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    for (UITouch* touch in touches)
    {
        [delegate sendTouchPack:3 withTouch:touch inView:self];
    }
}

@end
