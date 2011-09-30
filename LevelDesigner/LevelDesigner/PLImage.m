//
//  PLImage.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLImage.h"
#import "PLProperty.h"

static inline struct PLColor PLMakeColor(double r, double g, double b, double a)
{
    struct PLColor c;
    c.r =r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

@implementation PLImage

-(id)initFromProperty:(PLProperty*)prop
{
    if (self = [super initFromLua:NULL])
    {
        PLProperty *p;
        
        p = [prop propertyWithKey:@"class"];
        imageClass = p?[p.stringValue retain]:nil;

        p = [prop propertyWithKey:@"pos"];
        frame = p?p.rectValue:NSMakeRect(0, 0, 1, 1);
        
        p = [prop propertyWithKey:@"texCoord"];
        portion = p?p.rectValue:NSMakeRect(0, 0, 1, 1);

        p = [prop propertyWithKey:@"tag"];
        tag = p?p.numberValue:0;
        
        p = [prop propertyWithKey:@"rotation"];
        rotation = p?p.numberValue:0;
        
        p = [prop propertyWithKey:@"horizontallyFlipped"];
        horizontallyFlipped = p?p.booleanValue:NO;
        
        p = [prop propertyWithKey:@"verticallyFlipped"];
        verticallyFlipped = p?p.booleanValue:NO;
        
        p = [prop propertyWithKey:@"tint"];
        if (p && p.type==PLPropertyDictionary)
        {
            double r,g,b,a;
            r = [p propertyWithKey:@"r"].numberValue;
            g = [p propertyWithKey:@"g"].numberValue;
            b = [p propertyWithKey:@"b"].numberValue;
            a = [p propertyWithKey:@"a"].numberValue;
            tint = PLMakeColor(r, g, b, a);
        } else
            tint = PLMakeColor(-1,-1,-1,-1);
        
        p = [prop propertyWithKey:@"filename"];
        fileName = p?[p.stringValue retain]:nil;
        
        p = [prop propertyWithKey:@"alpha"];
        alpha = p?p.numberValue:1.0f;
        
    }
    return self;
}

+(NSArray*)imagesFromProperty:(PLProperty*)prop
{
    if (prop.type!=PLPropertyArray)
        return [NSArray array];
    NSMutableArray * a = [[[NSMutableArray alloc] initWithCapacity:[prop childrenCount]] autorelease];
    for (PLProperty * p in prop.arrayValue)
        [a addObject:[[[PLImage alloc] initFromProperty:p] autorelease]];
    return a;
}

@end
