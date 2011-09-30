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
@class PLPrototype;
@interface PLObject : PLEntity
{
    PLProperty * rootProperty;
    EntityController * subentityModel;
    PLPrototype * prototype;
    NSString * className;
}

@property(nonatomic,readonly) PLProperty * rootProperty;
@property(nonatomic,readonly) EntityController * subentityModel;
@property(nonatomic,readonly) NSString * className;
@property(nonatomic,readonly) PLPrototype * prototype;

-(void)propertyChanged:(PLProperty*)p;

-(PLProperty*)propertyAtKeyPath:(NSString*)keyPath;
-(void)writeToFile:(NSMutableString*)file;
-(NSUndoManager*)undoManager;


@end
