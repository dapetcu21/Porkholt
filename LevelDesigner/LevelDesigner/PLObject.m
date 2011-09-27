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

-(void)markMandatory
{
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
}

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
            rootProperty.name = @"__root__";
            rootProperty.dictionaryValue = [NSDictionary dictionary];
        }
        
        [self markMandatory];
                
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

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) 
    {
        rootProperty = [(PLProperty*)[aDecoder decodeObjectForKey:@"rootProperty"] retain];
        rootProperty.owner = self;
        [self markMandatory];
    }
    return self;
}

-(void)encodeWithCoder:(NSCoder *)aCoder
{
    [aCoder encodeObject:rootProperty forKey:@"rootProperty"];
}

-(NSString*)description
{
    NSString * className = [rootProperty propertyWithKey:@"class"].stringValue;
    PLProperty * p = [rootProperty propertyWithKey:@"scripting"];
    NSString * identifier;
    if (p)
        identifier = p.stringValue;
    else
        identifier = [NSString stringWithFormat:@"0x%x",self];
    return [NSString stringWithFormat:@"%@: %@",className,identifier];
}

-(void)dealloc
{
    [rootProperty release];
    [subentityModel release];
    [super dealloc];
}

-(void)propertyChanged:(PLProperty *)p
{
    NSString * name = p.name;
    if (p.parent==rootProperty && ([name isEqual:@"scripting"] || [name isEqual:@"class"]))
        [(EntityController*)owner entityDescriptionChanged:self];
}

-(void)setReadOnly:(BOOL)ro
{
    [super setReadOnly:ro];
    subentityModel.readOnly = ro;
}

@end
