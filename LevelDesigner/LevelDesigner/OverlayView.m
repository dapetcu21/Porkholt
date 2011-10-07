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

-(id)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self)
    {
        [self setAlphaValue:0.3f];
        trackingRect = [self addTrackingRect:[self bounds] owner:self userData:nil assumeInside:NO];
    }
    return self;
}

-(void)reshape
{
    [self removeTrackingRect:trackingRect];
    trackingRect = [self addTrackingRect:[self bounds] owner:self userData:nil assumeInside:NO];
}

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

-(void)_show
{
    [[self animator] setAlphaValue:1.0f];
    [[overlayController window] makeKeyWindow];
    if ([[overlayController window] firstResponder] == [overlayController window])
        [[overlayController window] makeFirstResponder:self];
}

-(void)_hide
{
    [[self animator] setAlphaValue:0.3f];
    [[overlayController parentWindow] makeKeyWindow];
}

-(void)show
{
    showcount++;
    if (showcount==1)
        [self _show];
}

-(void)hide
{
    if (showcount)
    {
        showcount=0;
        [self _hide];
    }
}

-(void)mouseEntered:(NSEvent *)theEvent
{
    [self show];
}

-(void)mouseExited:(NSEvent *)theEvent
{
    [self hide];
}

- (BOOL) acceptsFirstResponder { return YES; }
- (BOOL) becomeFirstResponder { return YES; }
- (BOOL) resignFirstResponder { return YES; }

-(void)undo:(id)sender
{
    if (!([[[overlayController parentWindow] undoManager] canUndo] && [[[overlayController parentWindow] firstResponder] tryToPerform:@selector(undo:) with:sender]))
        NSBeep();
}

-(void)redo:(id)sender
{
    if (!([[[overlayController parentWindow] undoManager] canRedo] && [[[overlayController parentWindow] firstResponder] tryToPerform:@selector(redo:) with:sender]))
        NSBeep();
}

@end
