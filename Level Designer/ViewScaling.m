//
//  ViewScaling.m
//  Level Designer
//
//  Created by Marius Petcu on 4/10/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ViewScaling.h"


@implementation NSView (ScaleUtilities)

static const NSSize unitSize = {1.0, 1.0};

// Returns the scale of the receiver's coordinate system, relative to the window's base coordinate system.
- (NSSize)scale;
{
    return [self convertSize:unitSize toView:nil];
}

// Sets the scale in absolute terms.
- (void)setScale:(NSSize)newScale;
{
    [self resetScaling]; // First, match our scaling to the window's coordinate system
    [self scaleUnitSquareToSize:newScale]; // Then, set the scale.
    [self setNeedsDisplay:YES]; // Finally, mark the view as needing to be redrawn
}

// Makes the scaling of the receiver equal to the window's base coordinate system.
- (void)resetScaling;
{
    [self scaleUnitSquareToSize:[self convertSize:unitSize fromView:nil]];
}

@end