//
//  PHAppDelegate.h
//  Porkholt
//
//  Created by Marius Petcu on 4/10/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#include "PHWindowing.h"

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
