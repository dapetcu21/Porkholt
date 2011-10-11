//
//  SubentityController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "EntityController.h"

#define SubentityController_numberOfArrays 2
#define PLSubentityPBoardType @"PLSubentityPBoardType"
#define PLSubentityLocationPBoardType @"PLSubentityLocationPBoardType"
#define PLSubentityPointerPBoardType @"PLSubentityPointerPBoardType"

@class PLEntity;
@class PLObject;

@interface SubentityController : EntityController
{
    PLEntity * owner;
}

-(id)initWithObject:(PLEntity*)e;
-(PLObject*)object;
-(NSArray*)images;
-(NSArray*)fixtures;

@end
