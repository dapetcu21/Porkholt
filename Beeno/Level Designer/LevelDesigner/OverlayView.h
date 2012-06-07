/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Cocoa/Cocoa.h>

enum OverlayViewCornerMask
{
    OverlayViewTopLeftCorner = (1<<0),
    OverlayViewTopRightCorner = (1<<1),
    OverlayViewBottomLeftCorner = (1<<2),
    OverlayViewBottomRightCorner = (1<<3),
    OverlayViewAllCorners = OverlayViewTopLeftCorner | OverlayViewTopRightCorner | OverlayViewBottomLeftCorner | OverlayViewBottomRightCorner
};

@class OverlayController;
@interface OverlayView : NSView
{
    int cornerMask : OverlayViewAllCorners;
    OverlayController * overlayController;
    int showcount;
    NSTrackingRectTag trackingRect;
}

@property(nonatomic,assign) int cornerMask;
@property(nonatomic,assign) OverlayController * overlayController;

-(void)reshape;
-(void)show;
-(void)hide;

@end
