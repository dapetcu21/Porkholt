//
//  PLObject.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLObject.h"
#import "SubentityController.h"

@implementation PLObject
@synthesize rootProperty;
@synthesize subentityModel;

-(id)initFromLua:(lua_State*)L;
{
    self = [super initFromLua:L];
    if (self) {
        //rootProperty = [[PLProperty alloc] init];
        subentityModel = [[SubentityController alloc] initWithObject:self];
    }
    
    return self;
}

-(void)dealloc
{
    [rootProperty release];
    [subentityModel release];
    [super dealloc];
}

@end
