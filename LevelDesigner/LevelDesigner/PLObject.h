//
//  PLObject.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"

@class PLProperty;
@class SubentityController;
@class PLPrototype;

#ifndef __cplusplus
#define PLObjectView void
#else
class PLObjectView;
#endif

@interface PLObject : PLEntity
{
    PLProperty * rootProperty;
    SubentityController * subentityModel;
    PLPrototype * prototype;
    NSString * className;
    PLObjectView * actor;
}

@property(nonatomic,readonly) PLProperty * rootProperty;
@property(nonatomic,readonly) SubentityController * subentityModel;
@property(nonatomic,readonly) NSString * className;
@property(nonatomic,readonly) PLPrototype * prototype;

-(void)propertyChanged:(PLProperty*)p;
-(void)subobjectChanged:(PLEntity*)so;
-(void)objectChanged;

-(void)move:(NSPoint)p;
-(void)rotate:(double)p;

-(PLProperty*)propertyAtKeyPath:(NSString*)keyPath;
-(void)writeToFile:(NSMutableString*)file;
-(NSUndoManager*)undoManager;

@property(nonatomic,assign) PLObjectView * actor;

@end
