//
//  PHStartGame.mm
//  Porkholt
//
//  Created by Marius Petcu on 2/7/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include "PHStartGame.h"
#include "PorkholtAppDelegate.h"

int PHStartGameFlags = 0;
void (*PHStartGameEntryPoint)(PHGameManager *) = NULL;
void * PHStartGameUD = NULL;

int PHStartGame(int argc, char * argv[], unsigned int resX, unsigned int resY, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    PHStartGameFlags = flags;
    PHStartGameEntryPoint = entryPoint;
    PHStartGameUD = ud;
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, NSStringFromClass([PorkholtAppDelegate class]));
    [pool release];
    return retVal;
}
