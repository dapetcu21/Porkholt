//
//  OpenGLTimer.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PLPorkholtView;
@interface OpenGLTimer : NSObject
{
    NSMutableSet * views;
    NSTimer * timer;
}

-(void)addView:(PLPorkholtView*)v;
-(void)removeView:(PLPorkholtView*)v;

+(OpenGLTimer*)retainedInstance;
+(OpenGLTimer*)sharedInstance;

@end
