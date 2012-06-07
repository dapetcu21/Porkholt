/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
