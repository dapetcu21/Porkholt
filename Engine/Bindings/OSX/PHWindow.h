//
//  PHWindow.h
//  Porkholt
//
//  Created by Marius Petcu on 4/11/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHGameManager.h"
#include "PHWindowing.h"
#include "PHGLView.h"
#import <AppKit/AppKit.h>

#ifndef NSAppKitVersionNumber10_7 
#define NSAppKitVersionNumber10_7 1110 
#endif

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

@end
