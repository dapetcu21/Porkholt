//
//  PLFixture.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLFixture.h"
#import "PLProperty.h"

@implementation PLFixture

-(id)initFromProperty:(PLProperty*)prop
{
    if (self = [super initFromLua:NULL])
    {
        
    }
    return self;
}

+(NSArray*)fixturesFromProperty:(PLProperty*)prop
{
    if (prop.type!=PLPropertyArray)
        return [NSArray array];
    NSMutableArray * a = [[[NSMutableArray alloc] initWithCapacity:[prop childrenCount]] autorelease];
    for (PLProperty * p in prop.arrayValue)
        [a addObject:[[[PLFixture alloc] initFromProperty:p] autorelease]];
    return a;
}

@end
