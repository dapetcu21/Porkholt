//
//  PLObject.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"

@class PLProperty;
@class EntityController;
@interface PLObject : PLEntity
{
    PLProperty * rootProperty;
    EntityController * subentityModel;
}

@property(nonatomic,readonly) PLProperty * rootProperty;
@property(nonatomic,readonly) EntityController * subentityModel;

-(void)propertyChanged:(PLProperty*)p;

@end
