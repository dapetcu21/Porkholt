//
//  JointController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PLJoint;
@class PLObjectSelector;
@interface JointController : NSObject
{
    PLJoint * model;
    IBOutlet NSView * view;
    IBOutlet NSPopUpButton * typeB;
    IBOutlet PLObjectSelector * body1, * body2;
    IBOutlet NSButton * worldCoord, * collideConn;
    IBOutlet NSView * placeView, * distanceView, * motorView;
    IBOutlet NSTextField * anchor1x, * anchor1y;
    IBOutlet NSTextField * anchor2x, * anchor2y;
    IBOutlet NSTextField * anchorx, * anchory;
    IBOutlet NSTextField * freq, * damp;
    IBOutlet NSButton * enableLimit;
    IBOutlet NSTextField * upperLimit, * lowerLimit;
    IBOutlet NSButton * enableMotor;
    IBOutlet NSTextField * maxPower, * motorSpeed;
    IBOutlet NSTextField * axisLabel, * axisMeter;
    IBOutlet NSSlider * axisSlider;
}

-(void)jointChanged;
-(IBAction)controlsChanged:(id)sender;

@property(nonatomic,retain) PLJoint * model;
@end
