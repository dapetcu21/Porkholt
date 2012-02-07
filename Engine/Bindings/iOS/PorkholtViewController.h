//
//  PorkholtViewController.h
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Porkholt Labs!. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

class PHMutex;
class PHThread;
class PHGameManager;
@class EAGLView;

extern PHGameManager * PHGameManagerSingleton;

@interface PorkholtViewController : UIViewController
{
    BOOL animating;
	
    CADisplayLink * dl;
    NSTimer * timer;
    EAGLView * v;
    NSConditionLock * exitCondition;
	NSThread * thread;
    int fps;
    
    PHGameManager * gameManager;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;

- (void)startAnimation;
- (void)stopAnimation;

-(PHGameManager*)gameManager;

@end
