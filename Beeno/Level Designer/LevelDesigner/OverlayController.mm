/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "OverlayController.h"
#import "PLPorkholtView.h"
#import "OverlayWindow.h"
#import "OverlayView.h"

@implementation OverlayController
@synthesize parentWindow;
@synthesize window;

-(OverlayView*)bottomView
{
    return bottomView;
}

-(OverlayView*)rightView
{
    return rightView;
}

-(void)addBottomView
{
    if (!window) return;
    if (!bottomView) return;
    NSRect pos = [[window contentView] bounds];
    pos.size = [bottomView frame].size;
    [[window contentView] addSubview:bottomView];
    [bottomView setFrame:pos];
    [bottomView setAutoresizingMask:NSViewMaxXMargin | NSViewMaxYMargin];
    [bottomView setCornerMask:OverlayViewTopRightCorner];
}

-(void)addRightView
{
    if (!window) return;
    if (!rightView) return;
    NSRect pos = [[window contentView] bounds];
    NSSize s = [rightView frame].size;
    pos.origin.x = pos.origin.x+pos.size.width-s.width;
    pos.origin.y = pos.origin.y+pos.size.height-s.height;
    pos.size = s;
    [[window contentView] addSubview:rightView];
    [rightView setFrame:pos];
    [rightView setAutoresizingMask:NSViewMinXMargin | NSViewMinYMargin];
    [rightView setCornerMask:OverlayViewBottomLeftCorner];
}


-(void)setBottomView:(OverlayView *)_bottomView
{
    bottomView.overlayController = nil;
    _bottomView.overlayController = self;
    [_bottomView retain];
    [bottomView release];
    bottomView = _bottomView;
    [self addBottomView];
}

-(void)setRightView:(OverlayView *)_rightView
{
    rightView.overlayController = nil;
    _rightView.overlayController = self;
    [_rightView retain];
    [rightView release];
    rightView = _rightView;
    [self addRightView];
}

-(void)reshapeToRect:(NSRect)rect
{
    bounds = rect;
    rect.origin = [parentWindow convertBaseToScreen:rect.origin];
    [window setFrame:rect display:YES];
    rect.origin = NSZeroPoint;
    [[window contentView] setFrame:rect];
    [rightView reshape];
    [bottomView reshape];
    [openGLView makeCurrent];
    [openGLView render];
}

-(id)init
{
    self = [super init];
    if (self)
        bounds = NSMakeRect(300, 300, 100, 100);
    return self;
}

-(void)awakeFromNib
{
    window = [[OverlayWindow alloc] initWithContentRect:bounds styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:NO];
    [parentWindow addChildWindow:window ordered:NSWindowAbove];
    NSView * rootView = [[NSView alloc] init];
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor clearColor]];
    [window setContentView:rootView];
    [window setReleasedWhenClosed:YES];
    [window setAcceptsMouseMovedEvents:YES];
    //[window setDelegate:self];
    [parentWindow setAcceptsMouseMovedEvents:YES];
    [window orderWindow:NSWindowAbove relativeTo:[parentWindow windowNumber]];
    [self reshapeToRect:bounds];
    [self addBottomView];
    [self addRightView];
}

-(void)dealloc
{
    [window release];
    [rightView release];
    [bottomView release];
    [openGLView setOverlay:nil];
    [super dealloc];
}

/*
-(void)undo:(id)sender
{
    if (!([[parentWindow undoManager] canUndo] && [[parentWindow firstResponder] tryToPerform:@selector(undo:) with:sender]))
        NSBeep();
}

-(void)redo:(id)sender
{
    if (!([[parentWindow undoManager] canRedo] && [[parentWindow firstResponder] tryToPerform:@selector(redo:) with:sender]))
        NSBeep();
}*/


@end
