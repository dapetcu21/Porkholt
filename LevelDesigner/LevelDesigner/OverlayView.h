//
//  OverlayView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
}

@property(nonatomic,assign) int cornerMask;
@property(nonatomic,assign) OverlayController * overlayController;

@end
