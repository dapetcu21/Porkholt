/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#include <pthread.h>

@class EAGLView;
@protocol EAGLViewDelegate<NSObject>
-(void)eaGLViewReshaped:(EAGLView*)v;
-(void)eaGLViewCreatedFramebuffer:(EAGLView*)v;
@end

@interface EAGLView : UIView
{
@private
    id<EAGLViewDelegate> delegate;
    EAGLContext *context;
    GLuint defaultFramebuffer, colorRenderbuffer;
}

@property (nonatomic, assign) id<EAGLViewDelegate> delegate;
@property (nonatomic, readonly) EAGLContext *context;
@property (nonatomic, readonly) CGSize framebufferSize;
@property (nonatomic, readonly) GLuint defaultFramebuffer;
@property (nonatomic, readonly) GLuint colorRenderbuffer;

- (void)initMain;
- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end
