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
#import "SubentityController.h"
#import "ImageViewController.h"
#import "PLObject.h"

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
@synthesize viewController;

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
    [[self object] subobjectChanged:self];
    [viewController imageChanged];
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

-(NSUndoManager*)undoManager
{
    return [(EntityController*)owner undoManager];
}

-(void)setImageClass:(NSString *)cls
{
    if ([imageClass isEqual:cls]) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setImageClass:imageClass];
    [cls retain];
    [imageClass release];
    imageClass = cls;
    [(EntityController*)owner entityDescriptionChanged:self];
    [self imageChanged];
}

-(void)setFileName:(NSString *)fn
{
    if ([fn isEqual:fileName]) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setFileName:fileName];
    [fn retain];
    [fileName release];
    fileName = fn;
    [(EntityController*)owner entityDescriptionChanged:self];
    [self imageChanged];
}

-(void)setPortion:(NSRect)p
{
    if (NSEqualRects(portion, p)) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setPortion:portion];
    portion = p;
    [self imageChanged];
}

-(void)setFrame:(NSRect)fr
{
    if (NSEqualRects(fr, frame)) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setFrame:frame];
    frame = fr;
    [self imageChanged];
}

-(void)setTag:(int)t
{
    if (tag == t) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setTag:tag];
    tag= t;
    [self imageChanged];
}

-(void)setRotation:(double)rot
{
    if (rotation == rot) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setRotation:rotation];
    rotation = rot;
    [self imageChanged];
}

-(void)setHorizontallyFlipped:(BOOL)h
{
    if (horizontallyFlipped == h) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setHorizontallyFlipped:horizontallyFlipped];
    horizontallyFlipped = h;
    [self imageChanged];
}

-(void)setVerticallyFlipped:(BOOL)v
{
    if (verticallyFlipped==v) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setVerticallyFlipped:verticallyFlipped];
    verticallyFlipped = v;
    [self imageChanged];
}

-(void)setTint:(struct PLColor)t
{
    if (t.r == tint.r && t.g == tint.g && t.b == tint.b && t.a == tint.a) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setTint:t];
    tint = t;
    [self imageChanged];
}

-(void)setAlpha:(double)a
{
    if (a==alpha) return;
    [(PLImage*)[[self undoManager] prepareWithInvocationTarget:self] setAlpha:alpha];
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

-(PLObject*)object
{
    return (PLObject*)[(SubentityController*)owner object];
}

@end
