//
//  PorkholtViewController.h
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Home. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

class PHMutex;
class PHThread;

@interface PorkholtViewController : UIViewController
{
    BOOL animating;
	volatile BOOL running;
	
	PHMutex * pausemutex;
	PHThread * thread;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;

- (void)startAnimation;
- (void)stopAnimation;

@end
