//
//  JointController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "JointController.h"
#import "PLJoint.h"
#import "PLObjectSelector.h"

@implementation JointController

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(PLJoint*)model
{
    return model;
}

#define setNumberField(f,n) [(f) setStringValue:[NSString stringWithFormat:@"%.4lf",(double)(n)]]
#define setIntField(f,n) [(f) setStringValue:[NSString stringWithFormat:@"%d",(int)(n)]]
#define setBoolField(f,n) [(f) setState:(n)?NSOnState:NSOffState]

-(void)populateView
{
    [motorView removeFromSuperview];
    [distanceView removeFromSuperview];
    BOOL enabled = model && !model.readOnly;
    
    [typeB setEnabled:enabled];
    [body1 setEnabled:enabled];
    [body2 setEnabled:enabled];
    [worldCoord setEnabled:enabled];
    [collideConn setEnabled:enabled];
    [anchor1x setEnabled:enabled];
    [anchor1y setEnabled:enabled];
    [anchor2x setEnabled:enabled];
    [anchor2y setEnabled:enabled];
    [anchorx setEnabled:enabled];
    [anchory setEnabled:enabled];
    [freq setEnabled:enabled];
    [damp setEnabled:enabled];
    [collideConn setEnabled:enabled];
    [enableLimit setEnabled:enabled];
    [upperLimit setEnabled:enabled];
    [lowerLimit setEnabled:enabled];
    [enableMotor setEnabled:enabled];
    [maxPower setEnabled:enabled];
    [motorSpeed setEnabled:enabled];
    [axisSlider setEnabled:enabled];
    
    if (!model)
        return;
    NSView * v = NULL;
    if (model.type == PLPrismaticJoint || model.type == PLRevoluteJoint)
        v = motorView;
    else
        v = distanceView;
    [v setFrame:[placeView bounds]];
    [placeView addSubview:v];
    
    [typeB selectItemWithTag:model.type];
    setBoolField(worldCoord, model.worldCoord);
    setBoolField(collideConn, model.collideConnected);
    [body1 setModel:(ObjectController*)model.owner];
    [body2 setModel:(ObjectController*)model.owner];
    [body1 setSelectedObject:model.body1];
    [body2 setSelectedObject:model.body2];
    
    if (model.type == PLPrismaticJoint || model.type == PLRevoluteJoint)
    {
        setNumberField(anchorx, model.anchor1.x);
        setNumberField(anchory, model.anchor1.y);
        setBoolField(enableLimit, model.limitEnabled);
        setBoolField(enableMotor, model.motorEnabled);
        setNumberField(motorSpeed, model.motorSpeed);
        setNumberField(maxPower, model.motorMaxPower);
        setNumberField(lowerLimit, model.lowerValue);
        setNumberField(upperLimit, model.upperValue);
        [enableLimit setTitle:(model.type == PLPrismaticJoint)?@"Translation limit":@"Rotation limit"];
        BOOL hd = (model.type == PLRevoluteJoint);
        [axisLabel setHidden:hd];
        [axisMeter setHidden:hd];
        [axisSlider setHidden:hd];
        NSPoint p = model.axis;
        double l = sqrt(p.x*p.x+p.y*p.y);
        if (!l)
        {
            p = NSMakePoint(0, 1);
            l = 1;
        }
        p.x/=l;
        p.y/=l;
        double a = M_PI_2;
        if (fabs(p.x)<fabs(p.y))
        {
            a = acos(p.x);
            if (p.y<0)
                a = M_PI*2-a;
        } else {
            a = asin(p.y);
            if (p.x<0)
                a = M_PI-a;
            if (a<0)
                a+=2*M_PI;
        }
        a=a/M_PI*180;
        double sl  = 90-a;
        if (sl<0)
            sl+=360;
        [axisSlider setDoubleValue:sl];
        [axisMeter setStringValue:[NSString stringWithFormat:@"%lf %s",a,(a==1)?"degree":"degrees"]];
    }
    else {
        setNumberField(anchor1x, model.anchor1.x);
        setNumberField(anchor1y, model.anchor1.y);
        setNumberField(anchor2x, model.anchor2.x);
        setNumberField(anchor2y, model.anchor2.y);
        setNumberField(freq, model.frequency);
        setNumberField(damp, model.dampening);
    }
}

-(void)jointChanged
{
    [self populateView];
}

-(IBAction)controlsChanged:(id)sender
{
    if (sender == typeB)
    {
        [model setType:(int)[typeB selectedTag]];
    }
    if (sender == anchor1x || sender == anchor1y)
        [model setAnchor1:NSMakePoint([anchor1x doubleValue], [anchor1y doubleValue])];
    if (sender == anchor2x || sender == anchor2y)
        [model setAnchor2:NSMakePoint([anchor2x doubleValue], [anchor2y doubleValue])];
    if (sender == anchorx || sender == anchory)
        [model setAnchor1:NSMakePoint([anchorx doubleValue], [anchory doubleValue])];
    if (sender == worldCoord)
        [model setWorldCoord:[worldCoord state]==NSOnState];
    if (sender == collideConn)
        [model setCollideConnected:[collideConn state]==NSOnState];
    if (sender == freq)
        [model setFrequency:[freq doubleValue]];
    if (sender == damp)
        [model setDampening:[damp doubleValue]];
    if (sender == enableLimit)
        [model setLimitEnabled:[enableLimit state]==NSOnState];
    if (sender == upperLimit)
        [model setUpperValue:[upperLimit doubleValue]];
    if (sender == lowerLimit)
        [model setLowerValue:[lowerLimit doubleValue]];
    if (sender == enableMotor)
        [model setMotorEnabled:[enableMotor state]==NSOnState];
    if (sender == maxPower)
        [model setMotorMaxPower:[maxPower doubleValue]];
    if (sender == motorSpeed)
        [model setMotorSpeed:[motorSpeed doubleValue]];
    if (sender == body1)
        [model setBody1:[body1 selectedObject]];
    if (sender == body2)
        [model setBody1:[body2 selectedObject]];
    if (sender == axisSlider)
    {
        double a = -([axisSlider doubleValue]-90);
        a=a/180*M_PI;
        [model setAxis:NSMakePoint(cos(a), sin(a))];
    }
}

-(void)setModel:(PLJoint *)m
{
    model.controller = nil;
    m.controller = self;
    [m retain];
    [model release];
    model = m;
    [self populateView];
}

@end
