/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include <Porkholt/Core/PHWindowing.h>

class PHGameManager;
@interface PHAppDelegate : NSObject<NSApplicationDelegate, NSWindowDelegate>
{
    NSWindow * window;
    
    int flags;
    void (*entryPoint)(PHGameManager *);
    void * ud;
    PHWVideoMode vm;
}

-(id)initWithVM:(const PHWVideoMode &) vm flags:(int)flags entryPoint:(void (*)(PHGameManager *))entryPoint ud:(void*)ud;

@end
