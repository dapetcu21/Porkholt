//
//  EAGLView.m
//  Porkholt
//
//  Created by Marius Petcu on 12/15/10.
//  Copyright 2010 Home. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@dynamic context;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
	if (self)
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
		pthread_mutex_init(&mutex, NULL);
    }
    
    return self;
}

- (void)dealloc
{
    [self deleteFramebuffer];    
	[context release];
	if (context!=workingContext)
		[workingContext release];
	pthread_mutex_destroy(&mutex);
    [super dealloc];
}


- (void)initMain
{
	pthread_mutex_lock(&mutex);
	EAGLContext *aContext = nil; //[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2]; No GLES 2.0 ... yet...
    
    if (!aContext)
    {
        aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    }
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
	context = aContext;
	workingContext = context;
 	[self setFramebuffer];
	pthread_mutex_unlock(&mutex);
}

- (void) initSecondary
{
	pthread_mutex_lock(&mutex);
	EAGLContext *aContext = nil; //[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:context.sharegroup]; No GLES 2.0 ... yet...
    
    if (!aContext)
    {
        aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1 sharegroup:context.sharegroup];
    }
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
	workingContext = aContext;
	pthread_mutex_unlock(&mutex);
}

- (void)createFramebuffer
{
	pthread_mutex_lock(&mutex);
	if (context && !defaultFramebuffer)
	{
		[EAGLContext setCurrentContext:context];
		
		// Create default framebuffer object.
		glGenFramebuffers(1, &defaultFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		
		// Create color render buffer and allocate backing store.
		glGenRenderbuffers(1, &colorRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		BOOL res = [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
		
		NSLog(@"res:%d W:%d H:%d",res,framebufferWidth,framebufferHeight);
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
	}
	pthread_mutex_unlock(&mutex);
}

- (void)deleteFramebuffer
{
	pthread_mutex_lock(&mutex);
	if (context)
	{
		[EAGLContext setCurrentContext:context];
		
		if (defaultFramebuffer)
		{
			glDeleteFramebuffers(1, &defaultFramebuffer);
			defaultFramebuffer = 0;
		}
		
		if (colorRenderbuffer)
		{
			glDeleteRenderbuffers(1, &colorRenderbuffer);
			colorRenderbuffer = 0;
		}
	}
	pthread_mutex_unlock(&mutex);
}

- (void)setFramebuffer
{
	pthread_mutex_lock(&mutex);
	if (workingContext)
	{
		[EAGLContext setCurrentContext:workingContext];
		
		if (!defaultFramebuffer)
		{
			pthread_mutex_unlock(&mutex);
			[self createFramebuffer];
			//[self performSelectorOnMainThread:@selector(createFramebuffer) withObject:nil waitUntilDone:YES];
			pthread_mutex_lock(&mutex);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		
		glViewport(0, 0, framebufferWidth, framebufferHeight);
	}
	pthread_mutex_unlock(&mutex);
}

- (BOOL)presentFramebuffer
{
	BOOL success = FALSE;
	pthread_mutex_lock(&mutex);
	if (workingContext)
	{
		[EAGLContext setCurrentContext:workingContext];
		
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		
		success = [workingContext presentRenderbuffer:GL_RENDERBUFFER];
	}
	pthread_mutex_unlock(&mutex);
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    [self deleteFramebuffer];
	//[self createFramebuffer];
}

@end
