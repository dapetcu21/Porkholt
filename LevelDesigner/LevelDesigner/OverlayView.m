//
//  OverlayView.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "OverlayView.h"
#import "OverlayController.h"

@implementation OverlayView
@synthesize cornerMask;
@synthesize overlayController;

- (void)drawRect:(NSRect)dirtyRect
{
    NSRect bounds = [self bounds];
    double radiusX = 10.0f;
    double radiusY = 10.0f;
    [[NSColor windowBackgroundColor] setFill];
    NSBezierPath * path = [NSBezierPath bezierPathWithRoundedRect:bounds xRadius:radiusX yRadius:radiusY];
    [path fill];
    if (!(cornerMask & OverlayViewBottomLeftCorner))
        [[NSBezierPath bezierPathWithRect:NSMakeRect(0, 0, radiusX, radiusY)] fill];
    if (!(cornerMask & OverlayViewBottomRightCorner))
        [[NSBezierPath bezierPathWithRect:NSMakeRect(bounds.size.width-radiusY, 0, radiusX, radiusY)] fill];
    if (!(cornerMask & OverlayViewTopLeftCorner))
        [[NSBezierPath bezierPathWithRect:NSMakeRect(0, bounds.size.height-radiusY, radiusX, radiusY)] fill];
    if (!(cornerMask & OverlayViewTopRightCorner))
        [[NSBezierPath bezierPathWithRect:NSMakeRect(bounds.size.width-radiusY, bounds.size.height-radiusY, radiusX, radiusY)] fill];
}

-(void)mouseEntered:(NSEvent *)theEvent
{
    [overlayController show];
}

-(void)mouseExited:(NSEvent *)theEvent
{
    [overlayController hide];
}
@end
