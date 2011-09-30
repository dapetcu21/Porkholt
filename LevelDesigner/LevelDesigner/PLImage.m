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

-(void)writeToFile:(NSMutableString*)file
{
    [file appendFormat:@"objectAddImage(obj,[[%@]],%lf,%lf,%lf,%lf",fileName,frame.origin.x,frame.origin.y,frame.size.width,frame.size.height];
    int count = 0;
    if (imageClass && ![imageClass isEqual:@""])
        addToken(file,[NSString stringWithFormat:@"class = [[%@]]",imageClass],&count);
    if (!NSEqualRects(portion, NSMakeRect(0, 0, 1, 1)))
        addToken(file,[NSString stringWithFormat:@"texCoord = rect(%lf,%lf,%lf,%lf)",portion.origin.x,portion.origin.y,portion.size.width,portion.size.height],&count);
    if (tag)
        addToken(file,[NSString stringWithFormat:@"tag = %d",tag],&count);
    if (alpha!=1)
        addToken(file,[NSString stringWithFormat:@"alpha = %lf",alpha],&count);
    if (rotation)
        addToken(file,[NSString stringWithFormat:@"rotation = %lf",rotation],&count);
    if (horizontallyFlipped)
        addToken(file,@"horizontallyFlipped = true",&count);
    if (verticallyFlipped)
        addToken(file,@"verticallyFlipped = true",&count);
    if (tint.a>=0 && !(tint.r==1 && tint.g==1 && tint.b==1 && tint.a==1))
        addToken(file,[NSString stringWithFormat:@"tint = rgba(%lf,%lf,%lf,%lf)",tint.r,tint.g,tint.b,tint.a],&count);
    endToken(file, &count);
    [file appendString:@")\n"];
}

-(void)dealloc
{
    [fileName release];
    [imageClass release];
    [super dealloc];
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
