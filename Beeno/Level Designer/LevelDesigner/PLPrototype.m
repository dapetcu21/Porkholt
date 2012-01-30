//
//  PLPrototype.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLPrototype.h"

@implementation PLPrototype
@synthesize rootProperty;
@synthesize images;
@synthesize fixtures;

-(void)dealloc
{
    [rootProperty release];
    [images release];
    [fixtures release];
    [super dealloc];
}

@end
