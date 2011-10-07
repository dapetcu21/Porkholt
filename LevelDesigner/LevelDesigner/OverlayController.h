//
//  OverlayController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
