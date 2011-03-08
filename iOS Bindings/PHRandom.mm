/*
 *  PHRandom.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/8/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#import <QuartzCore/QuartzCore.h>

void * PHCreateGLContextAndBindIt()
{
	EAGLContext *aContext = nil; //[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2]; No GLES 2.0 ... yet...
    
    if (!aContext)
    {
        aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    }
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
	return aContext;
}

void PHDestroyGLContext(void * context)
{
	if ([EAGLContext currentContext] == (EAGLContext*) context)
        [EAGLContext setCurrentContext:nil];
    
    [(EAGLContext*)context release];
}
