/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <AppKit/AppKit.h>

class PHGameManager;
class PHView;
@class OverlayController;

#import "PLPorkholtViewDelegate.h"

@interface PLPorkholtView : NSOpenGLView
{
    PHGameManager * gameManager;
    NSTimer * timer;
    PHView * worldView;
    OverlayController * overlay;
    BOOL rotating;
    BOOL zooming;
    id<PLPorkholtViewDelegate> delegate;
}

-(PHGameManager*)gameManager;
-(PHView*)worldView;
-(IBAction)resetToOrigin:(id)sender;
-(void)render;
-(void)makeCurrent;
+(void)globalFrame;

@property(nonatomic,retain) IBOutlet OverlayController * overlay;
@property(nonatomic,assign) IBOutlet id<PLPorkholtViewDelegate> delegate;

-(IBAction)toggleShowMarkers:(id)sender;
-(IBAction)toggleShowImages:(id)sender;
-(IBAction)toggleShowFixtures:(id)sender;
-(IBAction)toggleShowJoints:(id)sender;
-(IBAction)match:(id)sender;

@end
