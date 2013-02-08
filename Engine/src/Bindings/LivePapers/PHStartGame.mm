/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include <Porkholt/Core/PHWindowing.h>
#include "LPPHViewController.h"

PHMAIN_TYPE PHWMain(PHMAIN_PROTO, const PHWVideoMode & vmode, int flags, void (*entryPoint)(PHGameManager *), void * ud)
{
    return [[LPPHViewController alloc] initWithFlags:flags entryPoint:entryPoint userData:ud initDictionary:(NSDictionary*)infoDictionary];
}
