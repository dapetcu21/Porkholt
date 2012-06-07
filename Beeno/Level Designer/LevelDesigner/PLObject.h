/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
    NSString * objectName;
}

@property(nonatomic,readonly) PLProperty * rootProperty;
@property(nonatomic,readonly) SubentityController * subentityModel;
@property(nonatomic,readonly) NSString * className;
@property(nonatomic,readonly) PLPrototype * prototype;
@property(nonatomic,retain) NSString * objectName;

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
