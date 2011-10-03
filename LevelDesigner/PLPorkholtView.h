//
//  PLPorkholtView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <AppKit/AppKit.h>

class PHGameManager;
@interface PLPorkholtView : NSOpenGLView
{
    PHGameManager * gameManager;
    NSTimer * timer;
}

-(PHGameManager*)gameManager;

@end
