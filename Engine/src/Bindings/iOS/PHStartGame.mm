/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include <Porkholt/Core/PHWindowing.h>
#include "PorkholtAppDelegate.h"

int PHWFlags = 0;
void (*PHWEntryPoint)(PHGameManager *) = NULL;
void * PHWUD = NULL;

int PHWMain(int argc, char * argv[], const PHWVideoMode & vmode, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHWFlags = flags;
    PHWEntryPoint = entryPoint;
    PHWUD = ud;
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([PorkholtAppDelegate class]));
    [pool release];
    return retVal;
}
