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
@interface OverlayController : NSObject
{
    OverlayView * bottomView, * rightView;
    NSRect bounds;
    NSWindow * window;
    NSWindow * parentWindow;
    IBOutlet PLPorkholtView * openGLView;
    int showcount;
    NSTrackingRectTag rightDummyTag, bottomDummyTag;
}

@property(nonatomic,retain) IBOutlet NSView * bottomView;
@property(nonatomic,retain) IBOutlet NSView * rightView;
@property(nonatomic,assign) IBOutlet NSWindow * parentWindow;

-(void)reshapeToRect:(NSRect)rect;

-(void)show;
-(void)hide;

@end
