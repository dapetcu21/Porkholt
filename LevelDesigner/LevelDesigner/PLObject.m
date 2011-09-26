//
//  PLObject.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLObject.h"
#import "SubentityController.h"
#import "PLProperty.h"

@implementation PLObject
@synthesize rootProperty;
@synthesize subentityModel;

-(id)initFromLua:(lua_State*)L;
{
    self = [super initFromLua:L];
    if (self) {
        if (L)
        {
            if (lua_istable(L, -1))
            {
                lua_getfield(L, -1, "rootProperty");
                if (lua_istable(L, -1))
                {
                    rootProperty = [[PLProperty alloc] initFromLua:L];
                }
                lua_pop(L,1);
            }
        }
        else
        {
            rootProperty = [[PLProperty alloc] initFromLua:NULL];
        }
        PLProperty * p;
        
        p = [rootProperty propertyWithKey:@"class"];
        if (!p)
        {
            p = [[[PLProperty alloc] init] autorelease];
            p.name = @"class";
            p.stringValue = @"PHLObject";
            [rootProperty insertProperty:p atIndex:0];
        }
        p.mandatory = YES;
        
        p = [rootProperty propertyWithKey:@"pos"];
        if (!p)
        {
            p = [[[PLProperty alloc] init] autorelease];
            p.name = @"pos";
            p.pointValue = NSZeroPoint;
            [rootProperty insertProperty:p atIndex:0];
        }
        p.mandatory = YES;
        
        p = [rootProperty propertyWithKey:@"rotation"];
        if (!p)
        {
            p = [[[PLProperty alloc] init] autorelease];
            p.name = @"rotation";
            p.numberValue = 0;
            [rootProperty insertProperty:p atIndex:0];
        }
        p.mandatory = YES;
        
        PLProperty * phy = [rootProperty propertyWithKey:@"physics"];
        PLProperty * fixtures = [[phy propertyWithKey:@"fixtures"] retain];
        [phy removeProperty:fixtures];
        if (![phy childrenCount])
            [rootProperty removeProperty:phy];
        
        PLProperty * images = [[rootProperty propertyWithKey:@"images"] retain];
        [rootProperty removeProperty:images];
        
        rootProperty.owner = self;
        subentityModel = [[SubentityController alloc] initWithObject:self];
        //[subentityModel addImagesFromProperty:images];
        //[subentityModel addFixturesFromProperty:fixtures];
        [images release];
        [fixtures release];
    }
    
    return self;
}

-(void)dealloc
{
    [rootProperty release];
    [subentityModel release];
    [super dealloc];
}

-(void)propertyChanged:(PLProperty *)p
{
    
}

@end
