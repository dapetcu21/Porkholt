/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <UIKit/UIKit.h>
#import <Porkholt/Core/PHMain.h>

class PHMutex;

@interface PHTouchInterface : UIView <UIAccelerometerDelegate> {
    float scale;
}

extern PHTouchInterface * PHTouchInterfaceSingleton;

@end
