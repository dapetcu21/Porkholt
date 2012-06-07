/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>

@class PLPorkholtView;
@class OverlayView;
@interface OverlayController : NSObject<NSWindowDelegate>
{
    OverlayView * bottomView, * rightView;
    NSRect bounds;
    NSWindow * window;
    NSWindow * parentWindow;
    IBOutlet PLPorkholtView * openGLView;
}

@property(nonatomic,retain) IBOutlet NSView * bottomView;
@property(nonatomic,retain) IBOutlet NSView * rightView;
@property(nonatomic,assign) IBOutlet NSWindow * parentWindow;
@property(nonatomic,readonly) NSWindow * window;
-(void)reshapeToRect:(NSRect)rect;

@end
