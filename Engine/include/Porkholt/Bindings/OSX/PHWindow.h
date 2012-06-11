/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHWindowing.h>
#include <Porkholt/Bindings/OSX/PHGLView.h>
#import <AppKit/AppKit.h>

#ifndef NSAppKitVersionNumber10_7 
#define NSAppKitVersionNumber10_7 1110 
#endif

//#define PH_OSX_FAKERESOLUTION

@interface PHWindow : NSWindow
{
    BOOL fullScreen;
    NSRect windowedFrame;
    NSUInteger windowedMask;
    
    BOOL resizable;
    PHWVideoMode vm;
    
    PHGLView * view;
    
    NSString * title;
}

-(id)initWithVM:(const PHWVideoMode &) vm flags:(int)flags entryPoint:(void (*)(PHGameManager *))entryPoint ud:(void*)ud;

@property(nonatomic, assign) PHWVideoMode videoMode;
@property(nonatomic, assign) BOOL resizable;
@property(nonatomic, readonly) PHGLView * view;

#ifndef PH_OSX_FAKERESOLUTION
+(void)setResolution:(PHWVideoMode &)vm;
+(void)restoreResolution;
+(void)undoRestoreResolution;
#endif

@end
