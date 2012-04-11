//
//  PHAppDelegate.h
//  Porkholt
//
//  Created by Marius Petcu on 4/10/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

class PHGameManager;
@interface PHAppDelegate : NSObject<NSApplicationDelegate, NSWindowDelegate>
{
    NSWindow * window;
    
    int flags;
    void (*entryPoint)(PHGameManager *);
    void * ud;
    unsigned int resX;
    unsigned int resY;
}

-(id)initWithResX:(unsigned int)resX resY:(unsigned int)resY flags:(int)flags entryPoint:(void (*)(PHGameManager *))entryPoint ud:(void*)ud;

@end
