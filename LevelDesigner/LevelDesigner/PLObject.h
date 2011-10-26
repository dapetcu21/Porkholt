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
@class PLBezier;

#ifndef __cplusplus
#else
#endif

#ifndef __cplusplus
#define PLObjectView void
#define PHBezierPath void
#else
class PLObjectView;
class PHBezierPath;
#endif

@interface PLObject : PLEntity
{
    PLProperty * rootProperty;
    SubentityController * subentityModel;
    PLPrototype * prototype;
    NSString * className;
    PLObjectView * actor;
    NSMutableArray * beziers;
}

@property(nonatomic,readonly) PLProperty * rootProperty;
@property(nonatomic,readonly) SubentityController * subentityModel;
@property(nonatomic,readonly) NSString * className;
@property(nonatomic,readonly) PLPrototype * prototype;

-(PLBezier*)bezierPathAtIndex:(int)index;

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
