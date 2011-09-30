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

static inline void addToken(NSMutableString * file,NSString * token, int * count)
{
    if (!*count)
        [file appendFormat:@",{ %@",token];
    else
        [file appendFormat:@", %@",token];
    (*count)++;
}

static inline void endToken(NSMutableString * file, int * count)
{
    if (*count)
        [file appendFormat:@" }"];
}

-(id)initFromProperty:(PLProperty*)prop
{
    if (self = [super initFromLua:NULL])
    {
        PLProperty *p;
        
        p = [prop propertyWithKey:@"box"];
        box = p?p.rectValue:NSMakeRect(-0.5f, -0.5f, 1, 1);
        
        p = [prop propertyWithKey:@"shape"];
        shape = PLFixtureRect;
        if ([p.stringValue isEqual:@"circle"])
            shape = PLFixtureCircle;
        
        p = [prop propertyWithKey:@"rotation"];
        rotation = p?p.numberValue:0;
        
        p = [prop propertyWithKey:@"circleR"];
        radius = p?p.numberValue:0;
        
        p = [prop propertyWithKey:@"pos"];
        position = p?p.pointValue:NSMakePoint(0, 0);
        
        p = [prop propertyWithKey:@"friction"];
        friction = p?p.numberValue:0.3f;
        
        p = [prop propertyWithKey:@"restitution"];
        restitution = p?p.numberValue:0.0f;
        
        p = [prop propertyWithKey:@"friction"];
        density = p?p.numberValue:1.0f;
        
        p = [prop propertyWithKey:@"groupIndex"];
        groupIndex = p?p.numberValue:0;
        
        p = [prop propertyWithKey:@"categoryBits"];
        categoryBits = p?p.numberValue:0x0001;
        
        p = [prop propertyWithKey:@"maskBits"];
        maskBits = p?p.numberValue:0xFFFF;
    }
    return self;
}

-(void)writeToFile:(NSMutableString*)file
{
    switch (shape) {
        case PLFixtureRect:
            [file appendFormat:@"objectAddBox(obj,%lf,%lf,%lf,%lf",box.origin.x,box.origin.y,box.size.width,box.size.height];
            break;
        case PLFixtureCircle:
            [file appendFormat:@"objectAddCircle(obj,%lf",radius];
            break;
        default:
            return;
    }
    
    int count = 0;
    if (rotation)
        addToken(file,[NSString stringWithFormat:@"rotation = %lf",rotation], &count);
    
    if (friction!=0.3f)
        addToken(file,[NSString stringWithFormat:@"friction = %lf",friction], &count);
    
    if (density!=1.0f)
        addToken(file,[NSString stringWithFormat:@"density = %lf",density], &count);

    if (restitution)
        addToken(file,[NSString stringWithFormat:@"restitution = %lf",restitution], &count);
    
    if (groupIndex)
        addToken(file,[NSString stringWithFormat:@"groupIndex = %d",groupIndex],&count);
    
    if (categoryBits!=0x0001)
        addToken(file,[NSString stringWithFormat:@"categoryBits = %d",categoryBits],&count);
    
    if (maskBits!=0xFFFF)
        addToken(file,[NSString stringWithFormat:@"maskBits = %d",maskBits],&count);
    
    endToken(file, &count);
    [file appendString:@")\n"];
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
