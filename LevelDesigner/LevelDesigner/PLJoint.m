//
//  PLJoint.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLJoint.h"
#import "EntityController.h"

@implementation PLJoint

- (id)initFromLua:(lua_State *)L
{
    self = [super initFromLua:L];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(void)jointChanged
{
    [(EntityController*)owner entityChanged:self];
}

@end
