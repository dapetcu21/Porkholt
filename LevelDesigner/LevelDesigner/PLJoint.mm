//
//  PLJoint.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLJoint.h"
#import "EntityController.h"
#import "PHLua.h"
#import "PLObject.h"
#import "JointController.h"

@implementation PLJoint
@synthesize body1index;
@synthesize body2index;
@synthesize controller;

- (id)initFromLua:(lua_State *)L
{
    self = [super initFromLua:L];
    if (self) {
        type = PLDistanceJoint;
        body1 = body2 = nil;
        worldCoord = NO;
        collideConnected = YES;
        anchor1 = NSMakePoint(0, 0);
        anchor2 = NSMakePoint(0, 0);
        frequency = 0.0f;
        dampening = 0.0f;
        motorEnabled = NO;
        motorMaxPower = 1.0f;
        motorSpeed = 1.0f;
        lowerValue = upperValue = 0;
        limitEnabled = NO;
        axis = NSMakePoint(0, 1);
        body1index = body2index = -1;
        if (L && lua_istable(L, -1))
        {
            lua_getfield(L, -1, "class");
            NSString * clss = nil;
            if (lua_isstring(L, -1))
                clss = [NSString stringWithUTF8String:lua_tostring(L, -1)];
            lua_pop(L,1);
            if ([clss isEqual:@"PHDistanceJoint"])
                type = PLDistanceJoint;
            if ([clss isEqual:@"PHRevoluteJoint"])
                type = PLRevoluteJoint;
            if ([clss isEqual:@"PHPrismaticJoint"])
                type = PLPrismaticJoint;
            
            PHLuaGetBoolField(worldCoord, "worldCoordinates");
            PHLuaGetBoolField(collideConnected, "collideConnected");
            
            bool lvlDes = false;
            PHLuaGetBoolField(lvlDes,"levelDes");
            self.readOnly = !lvlDes;
            
            lua_getfield(L, -1, "body1");
            if (lua_istable(L, -1))
            {
                lua_getfield(L, -1, "index");
                if (lua_isnumber(L, -1))
                    body1index = lua_tonumber(L, -1);
                lua_pop(L,1);
            }
            lua_pop(L,1);
            
            lua_getfield(L, -1, "body2");
            if (lua_istable(L, -1))
            {
                lua_getfield(L, -1, "index");
                if (lua_isnumber(L, -1))
                    body2index = lua_tonumber(L, -1);
                lua_pop(L,1);
            }
            lua_pop(L,1);

            if (type == PLDistanceJoint)
            {
                lua_getfield(L, -1, "anchor1");
                if (lua_istable(L, -1))
                {
                    PHLuaGetNumberField(anchor1.x,"x");
                    PHLuaGetNumberField(anchor1.y,"y");
                }
                lua_pop(L,1);
                
                lua_getfield(L, -1, "anchor2");
                if (lua_istable(L, -1))
                {
                    PHLuaGetNumberField(anchor2.x,"x");
                    PHLuaGetNumberField(anchor2.y,"y");
                }
                lua_pop(L,1);
                
                PHLuaGetNumberField(frequency,"frequency");
                PHLuaGetNumberField(dampening,"dampening");
            }
            if (type == PLRevoluteJoint || type == PLPrismaticJoint)
            {
                lua_getfield(L, -1, "anchor");
                if (lua_istable(L, -1))
                {
                    PHLuaGetNumberField(anchor1.x,"x");
                    PHLuaGetNumberField(anchor1.y,"y");
                }
                lua_pop(L,1);
                
                PHLuaGetBoolField(limitEnabled, "limitEnabled");
                PHLuaGetBoolField(motorEnabled,"motorEnabled");
                PHLuaGetNumberField(motorMaxPower,"motorMaxPower");
                PHLuaGetNumberField(motorSpeed,"motorSpeed");
            }
            if (type == PLRevoluteJoint)
            {                
                PHLuaGetNumberField(lowerValue, "lowerAngle");
                PHLuaGetNumberField(upperValue, "upperAngle");
            }
            if (type == PLPrismaticJoint)
            {
                PHLuaGetNumberField(lowerValue, "lowerTranslation");
                PHLuaGetNumberField(upperValue, "upperTranslation");
                lua_getfield(L, -1, "axis");
                
                if (lua_istable(L, -1))
                {
                    PHLuaGetNumberField(axis.x,"x");
                    PHLuaGetNumberField(axis.y,"y");
                }
                lua_pop(L,1);
            }
        }
    }
    
    return self;
}

-(void)dealloc
{
    [body1 release];
    [body2 release];
    [super dealloc];
}

-(void)writeToFile:(NSMutableString*)file
{
    NSString * clss;
    switch (type)
    {
        case PLRevoluteJoint:
            clss = @"PHRevoluteJoint";
            break;
        case PLPrismaticJoint:
            clss = @"PHPrismaticJoint";
            break;
        case PLDistanceJoint:
        default:
            clss = @"PHDistanceJoint";
    }
    [file appendFormat:@"\njoint = jointWithClass(\"%@\")\njoint.levelDes = true\n",clss];
    if (body1)
        [file appendFormat:@"joint.body1 = %@\n",[body1 objectName]];
    if (body2)
        [file appendFormat:@"joint.body2 = %@\n",[body2 objectName]];
    if (worldCoord)
        [file appendString:@"worldCoordinates = true\n"];
    if (!collideConnected)
        [file appendString:@"collideConnected = false\n"];
    if (type == PLDistanceJoint)
    {
        if (anchor1.x || anchor1.y)
            [file appendFormat:@"anchor1 = point(%lf,%lf)\n",anchor1.x,anchor1.y];
        if (anchor2.x || anchor2.y)
            [file appendFormat:@"anchor2 = point(%lf,%lf)\n",anchor2.x,anchor2.y];
        if (frequency)
            [file appendFormat:@"frequency = %lf\n",frequency];
        if (dampening)
            [file appendFormat:@"dampening = %lf\n",dampening];
    }
    if (type == PLRevoluteJoint || type == PLPrismaticJoint)
    {
        if (anchor1.x || anchor1.y)
            [file appendFormat:@"anchor = point(%lf,%lf)\n",anchor1.x,anchor1.y];
        if (limitEnabled)
            [file appendString:@"limitEnabled = true\n"];
        if (motorEnabled)
            [file appendString:@"motorEnabled = true\n"];
        if (motorMaxPower != 1.0f)
            [file appendFormat:@"motorMaxPower = %lf",motorMaxPower];
        if (motorSpeed != 1.0f)
            [file appendFormat:@"motorSpeed = %lf",motorSpeed];
    }
    if (type == PLRevoluteJoint)
    {
        if (lowerValue)
            [file appendFormat:@"lowerAngle = %lf",lowerValue];
        if (upperValue)
            [file appendFormat:@"upperAngle = %lf",upperValue];
    }
    if (type == PLPrismaticJoint)
    {
        if (lowerValue)
            [file appendFormat:@"lowerTranslation = %lf",lowerValue];
        if (upperValue)
            [file appendFormat:@"upperTranslation = %lf",upperValue];
    }
    [file appendFormat:@"addJoint(joint)\n"];
}

-(void)encodeWithCoder:(NSCoder *)coder
{
    id d[] = {body1,body2,owner};
    NSData * data = [NSData dataWithBytes:d length:sizeof(id)*3];
    [coder encodeObject:data forKey:@"bodies"];
    [coder encodeBool:worldCoord forKey:@"worldCoord"];
    [coder encodeBool:collideConnected forKey:@"collideConnected"];
    [coder encodeInt:type forKey:@"type"];
    [coder encodePoint:anchor1 forKey:@"anchor1"];
    [coder encodePoint:anchor2 forKey:@"anchor2"];
    [coder encodeDouble:lowerValue forKey:@"lowerValue"];
    [coder encodeDouble:upperValue forKey:@"upperValue"];
    [coder encodeBool:limitEnabled forKey:@"limitEnabled"];
    [coder encodePoint:axis forKey:@"axis"];
    [coder encodeBool:motorEnabled forKey:@"motorEnabled"];
    [coder encodeDouble:motorMaxPower forKey:@"motorMaxPower"];
    [coder encodeDouble:motorSpeed forKey:@"motorSpeed"];
    [coder encodeDouble:frequency forKey:@"frequency"];
    [coder encodeDouble:dampening forKey:@"dampening"];
}

-(id)initWithCoder:(NSCoder *)decoder
{
    self = [self initFromLua:NULL];
    if (self)
    {
        id rd[3] = {nil,nil,nil};
        NSData * dt = (NSData*)[decoder decodeObjectForKey:@"bodies"];
        
        const id * d = (const id *)[dt bytes];
        if ([dt length]<sizeof(id)*3)
            d = rd;
        
        body1 = d[0];
        body2 = d[1];
        bodyOwner = d[2];
        
        worldCoord = [decoder decodeBoolForKey:@"worldCoord"];
        collideConnected = [decoder decodeBoolForKey:@"collideConnected"];
        type = [decoder decodeIntForKey:@"type"];
        anchor1 = [decoder decodePointForKey:@"anchor1"];
        anchor2 = [decoder decodePointForKey:@"anchor2"];
        lowerValue = [decoder decodeDoubleForKey:@"lowerValue"];
        upperValue = [decoder decodeDoubleForKey:@"upperValue"];
        limitEnabled = [decoder decodeBoolForKey:@"limitEnabled"];
        axis = [decoder decodePointForKey:@"axis"];
        motorEnabled = [decoder decodeBoolForKey:@"motorEnabled"];
        motorMaxPower = [decoder decodeDoubleForKey:@"motorMaxPower"];
        motorSpeed = [decoder decodeDoubleForKey:@"motorSpeed"];
        frequency = [decoder decodeDoubleForKey:@"frequency"];
        dampening = [decoder decodeDoubleForKey:@"dampening"];
    }
    return self;
}

-(void)postPaste
{
    if (owner!=bodyOwner)
        body1=body2=nil;
    else
    {
        if (body1 && [bodyOwner containsEntity:body1 inArray:0])
            [body1 retain];
        else
            body1 = nil;
        if (body2 && [bodyOwner containsEntity:body2 inArray:0])
            [body2 retain];
        else
            body2 = nil;
    }
}

-(void)jointChanged
{
    [(EntityController*)owner entityChanged:self];
    [controller jointChanged];
}

#define getter(x,y) -(x)y { return y; }

getter(PLObject*,body1)
getter(PLObject*,body2)
getter(BOOL,worldCoord)
getter(BOOL,collideConnected)
getter(int,type)
getter(NSPoint,anchor1)
getter(NSPoint,anchor2)
getter(double,lowerValue)
getter(double,upperValue)
getter(BOOL,limitEnabled)
getter(NSPoint,axis)
getter(BOOL,motorEnabled)
getter(double,motorMaxPower)
getter(double,motorSpeed)
getter(double,frequency)
getter(double,dampening)

-(NSUndoManager*)undoManager
{
    return [(EntityController*)owner undoManager];
}

-(void)setBody1:(PLObject *)b
{
    if (b==body1) return;
    [(PLJoint*)[[self undoManager] prepareWithInvocationTarget:self] setBody1:body1];
    [b retain];
    [body1 release];
    body1 = b;
}

-(void)setBody2:(PLObject *)b
{
    if (b==body2) return;
    [(PLJoint*)[[self undoManager] prepareWithInvocationTarget:self] setBody1:body2];
    [b retain];
    [body2 release];
    body2 = b;
}

#define setter(type,method,field) \
-(void)method:(type)v \
{ \
    if (v==field) return; \
    [(PLJoint*)[[self undoManager] prepareWithInvocationTarget:self] method:field]; \
    field = v; \
    [self jointChanged]; \
}

setter(BOOL,setWorldCoord,worldCoord)
setter(BOOL,setCollideConnected,collideConnected)
setter(int,setType,type)

-(void)setAnchor1:(NSPoint)a
{
    if (NSEqualPoints(anchor1, a)) return;
    [(PLJoint*)[[self undoManager] prepareWithInvocationTarget:self] setAnchor1:anchor1];
    anchor1 = a;
    [self jointChanged];
}

-(void)setAnchor2:(NSPoint)a
{
    if (NSEqualPoints(anchor2, a)) return;
    [(PLJoint*)[[self undoManager] prepareWithInvocationTarget:self] setAnchor2:anchor2];
    anchor2 = a;
    [self jointChanged];
}

setter(double,setLowerValue,lowerValue)
setter(double,setUpperValue,upperValue)
setter(BOOL,setLimitEnabled,limitEnabled)

-(void)setAxis:(NSPoint)a
{
    if (NSEqualPoints(axis, a)) return;
    [(PLJoint*)[[self undoManager] prepareWithInvocationTarget:self] setAxis:axis];
    axis = a;
    [self jointChanged];
}

setter(BOOL,setMotorEnabled,motorEnabled)
setter(double,setMotorMaxPower,motorMaxPower)
setter(double,setMotorSpeed,motorSpeed)
setter(double,setFrequency,frequency)
setter(double,setDampening,dampening)

@end
