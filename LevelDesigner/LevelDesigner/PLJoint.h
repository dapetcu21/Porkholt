//
//  PLJoint.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"
@class ObjectController;
@class PLObject;
@class JointController;

enum PLJointTypes 
{
    PLDistanceJoint = 0,
    PLRevoluteJoint,
    PLPrismaticJoint
};

@interface PLJoint : PLEntity
{
    PLObject * body1, * body2;
    int body1index,body2index;
    BOOL worldCoord;
    BOOL collideConnected;
    int type;
    NSPoint anchor1,anchor2;
    double lowerValue,upperValue;
    BOOL limitEnabled;
    NSPoint axis;
    BOOL motorEnabled;
    double motorMaxPower;
    double motorSpeed;
    double frequency,dampening;
    ObjectController * bodyOwner;
    JointController * controller;
}

@property(nonatomic,assign) JointController * controller;
@property(nonatomic,retain) PLObject * body1;
@property(nonatomic,retain) PLObject * body2;
@property(nonatomic,assign) int body1index;
@property(nonatomic,assign) int body2index;
@property(nonatomic,assign) BOOL worldCoord;
@property(nonatomic,assign) BOOL collideConnected;
@property(nonatomic,assign) int type;
@property(nonatomic,assign) NSPoint anchor1;
@property(nonatomic,assign) NSPoint anchor2;
@property(nonatomic,assign) double lowerValue;
@property(nonatomic,assign) double upperValue;
@property(nonatomic,assign) BOOL limitEnabled;
@property(nonatomic,assign) NSPoint axis;
@property(nonatomic,assign) BOOL motorEnabled;
@property(nonatomic,assign) double motorMaxPower;
@property(nonatomic,assign) double motorSpeed;
@property(nonatomic,assign) double frequency;
@property(nonatomic,assign) double dampening;

-(void)writeToFile:(NSMutableString*)file;

@end
