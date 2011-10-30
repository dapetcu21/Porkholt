//
//  PLMatching.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/30/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PLEntity.h"

@protocol PLMatching <NSObject>
-(BOOL)matchWithEntity:(PLEntity<PLMatching>*)entity;
@end
