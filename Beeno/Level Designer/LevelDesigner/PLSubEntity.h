/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import "PLEntity.h"

@protocol PLSubEntity <NSObject>

-(void)move:(NSPoint)delta;
-(void)rotate:(double)ammount;

@end
