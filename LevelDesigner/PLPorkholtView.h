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
@interface PLPorkholtView : NSOpenGLView
{
    PHGameManager * gameManager;
    NSTimer * timer;
    PHView * worldView;
}

-(PHGameManager*)gameManager;
-(PHView*)worldView;

@end
