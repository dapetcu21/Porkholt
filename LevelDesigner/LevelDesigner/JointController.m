//
//  JointController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "JointController.h"
#import "PLJoint.h"

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

-(void)jointChanged
{
    
}

-(void)setModel:(PLJoint *)m
{
    model.controller = nil;
    m.controller = self;
    [m retain];
    [model release];
    model = m;
}

@end
