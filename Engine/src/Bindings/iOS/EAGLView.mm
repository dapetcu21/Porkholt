/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

#import "EAGLView.h"

extern int PHWFlags;
#include <Porkholt/Core/PHWindowing.h>

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@synthesize context;
@synthesize delegate;
@synthesize defaultFramebuffer;
@synthesize colorRenderbuffer;

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (id)initWithCoder:(NSCoder*)coder
{
	if ((self = [super initWithCoder:coder]))
    {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        
        eaglLayer.opaque = TRUE;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
                                        [NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
                                        kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
                                        nil];
    }
    return self;
}

- (void)dealloc
{
    [self deleteFramebuffer];    
    [context release];
    [super dealloc];
}

- (void)initMain
{
    if (context) return;

    if ([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")])
        [self setContentScaleFactor:[[UIScreen mainScreen] scale]];

    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!context && (PHWFlags & PHWGLES1))
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!context)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:context])
        NSLog(@"Failed to set ES context current");
}

- (void)createFramebuffer
{
	if (context && !defaultFramebuffer)
	{
		[EAGLContext setCurrentContext:context];
		
		glGenFramebuffers(1, &defaultFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		
		glGenRenderbuffers(1, &colorRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		BOOL res = [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
        else {
            if ([delegate respondsToSelector:@selector(eaGLViewCreatedFramebuffer:)])
                [delegate eaGLViewCreatedFramebuffer:self];
        }
	}
}

- (void)deleteFramebuffer
{
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
}

- (void)setFramebuffer
{
    if (!context)
        [self initMain];
    if (context)
    {
		[EAGLContext setCurrentContext:context];
		if (!defaultFramebuffer)
			[self createFramebuffer];
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
	}
}

- (BOOL)presentFramebuffer
{
	BOOL success = FALSE;
	if (context)
	{
		[EAGLContext setCurrentContext:context];
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		success = [context presentRenderbuffer:GL_RENDERBUFFER];
	}
    return success;
}

- (CGSize)framebufferSize
{
    if (!context || !defaultFramebuffer) return CGSizeMake(0, 0);
    [EAGLContext setCurrentContext:context];
    glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);

    GLint w,h;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &w) ;
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &h) ;
    return CGSizeMake(w, h);
}

- (void)layoutSubviews
{
    [self deleteFramebuffer];
    if ([delegate respondsToSelector:@selector(eaGLViewReshaped:)])
        [delegate eaGLViewReshaped:self];
}

@end
