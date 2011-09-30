//
//  PLImage.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLImage.h"
#import "PLProperty.h"
#import "EntityController.h"

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
        imageClass = p?[p.stringValue retain]:[@"PHImageView" retain];

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
    if (imageClass && ![imageClass isEqual:@""] && ![imageClass isEqual:@"PHImageView"])
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

-(void)encodeWithCoder:(NSCoder *)aCoder
{
    PLProperty * prop = [[[PLProperty alloc] init] autorelease];
    prop.name = @"tempProperty";
    prop.dictionaryValue = [NSDictionary dictionary];
    
    PLProperty * p;
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"class";
    p.stringValue = imageClass;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"filename";
    p.stringValue = fileName;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"pos";
    p.rectValue = frame;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"texCoord";
    p.rectValue = portion;
    [prop insertProperty:p atIndex:[prop childrenCount]];

    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"tag";
    p.numberValue = tag;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"rotation";
    p.numberValue = rotation;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"alpha";
    p.numberValue = alpha;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"horizontallyFlipped";
    p.numberValue = horizontallyFlipped;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"verticallyFlipped";
    p.numberValue = verticallyFlipped;
    [prop insertProperty:p atIndex:[prop childrenCount]];

    PLProperty *r,*g,*b,*a;
    r = [[[PLProperty alloc] init] autorelease];
    r.name = @"r";
    r.numberValue = tint.r;
    g = [[[PLProperty alloc] init] autorelease];
    g.name = @"g";
    g.numberValue = tint.g;
    b = [[[PLProperty alloc] init] autorelease];
    b.name = @"b";
    b.numberValue = tint.b;
    a = [[[PLProperty alloc] init] autorelease];
    a.name = @"a";
    a.numberValue = tint.a;
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"tint";
    p.numberValue = verticallyFlipped;
    [p insertProperties:[NSArray arrayWithObjects:r,g,b,a,nil] atIndex:0];
    [prop insertProperty:p atIndex:[prop childrenCount]];

    [aCoder encodeObject:prop forKey:@"property"];
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    PLProperty * p = (PLProperty*)[aDecoder decodeObjectForKey:@"property"];
    return [self initFromProperty:p];
}

-(void)dealloc
{
    [fileName release];
    [imageClass release];
    [super dealloc];
}

-(void)imageChanged
{
    
}

-(NSString*)imageClass
{
    return imageClass;
}

-(NSString*)fileName
{
    return fileName;
}

-(NSRect)portion
{
    return portion;
}

-(NSRect)frame
{
    return frame;
}

-(int)tag
{
    return tag;
}

-(double)rotation
{
    return rotation;
}

-(BOOL)horizontallyFlipped
{
    return horizontallyFlipped;
}

-(BOOL)verticallyFlipped
{
    return verticallyFlipped;
}

-(struct PLColor)tint
{
    return tint;
}

-(double)alpha
{
    return alpha;
}

-(void)setImageClass:(NSString *)cls
{
    [cls retain];
    [imageClass release];
    imageClass = cls;
    [(EntityController*)owner entityDescriptionChanged:self];
    [self imageChanged];
}

-(void)setFileName:(NSString *)fn
{
    [fn retain];
    [fileName release];
    fileName = fn;
    [(EntityController*)owner entityDescriptionChanged:self];
    [self imageChanged];
}

-(void)setPortion:(NSRect)p
{
    portion = p;
    [self imageChanged];
}

-(void)setFrame:(NSRect)fr
{
    frame = fr;
    [self imageChanged];
}

-(void)setTag:(int)t
{
    tag= t;
    [self imageChanged];
}

-(void)setRotation:(double)rot
{
    rotation = rot;
    [self imageChanged];
}

-(void)setHorizontallyFlipped:(BOOL)h
{
    horizontallyFlipped = h;
    [self imageChanged];
}

-(void)setVerticallyFlipped:(BOOL)v
{
    verticallyFlipped = v;
    [self imageChanged];
}

-(void)setTint:(struct PLColor)t
{
    tint = t;
    [self imageChanged];
}

-(void)setAlpha:(double)a
{
    alpha = a;
    [self imageChanged];
}

-(NSString*)description
{
    return [NSString stringWithFormat:@"%@: %@",imageClass,fileName];
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
