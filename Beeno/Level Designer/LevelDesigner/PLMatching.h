/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>
#import "PLEntity.h"

@protocol PLMatching <NSObject>
-(BOOL)matchWithEntity:(PLEntity<PLMatching>*)entity;
@end
