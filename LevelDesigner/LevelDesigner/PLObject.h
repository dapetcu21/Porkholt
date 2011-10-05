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

#ifndef __cplusplus
#define PLObjectView void
#else
class PLObjectView;
#endif

@interface PLObject : PLEntity
{
    PLProperty * rootProperty;
    EntityController * subentityModel;
    PLPrototype * prototype;
    NSString * className;
    PLObjectView * actor;
}

@property(nonatomic,readonly) PLProperty * rootProperty;
@property(nonatomic,readonly) EntityController * subentityModel;
@property(nonatomic,readonly) NSString * className;
@property(nonatomic,readonly) PLPrototype * prototype;

-(void)propertyChanged:(PLProperty*)p;

-(PLProperty*)propertyAtKeyPath:(NSString*)keyPath;
-(void)writeToFile:(NSMutableString*)file;
-(NSUndoManager*)undoManager;

@property(nonatomic,assign) PLObjectView * actor;

@end
