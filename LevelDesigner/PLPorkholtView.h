//
//  PLPorkholtView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <AppKit/AppKit.h>

class PHGameManager;
class PHView;
@class OverlayController;
@interface PLPorkholtView : NSOpenGLView
{
    PHGameManager * gameManager;
    NSTimer * timer;
    PHView * worldView;
    OverlayController * overlay;
}

-(PHGameManager*)gameManager;
-(PHView*)worldView;
-(IBAction)resetToOrigin:(id)sender;
-(void)render;

@property(nonatomic,retain) IBOutlet OverlayController * overlay;

@end
