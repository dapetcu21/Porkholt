//
//  PLFixture.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLFixture.h"
#import "PLProperty.h"
#import "EntityController.h"
#import "SubentityController.h"
#import "FixtureViewController.h"
#import "PLObject.h"
#import "PLFixtureView.h"
#import "PLBezier.h"

@implementation PLFixture
@synthesize viewController;
@synthesize actor;
@synthesize bezierCurveIndex;

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

-(id)init
{
    return [self initFromProperty:NULL];
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
        if ([p.stringValue isEqual:@"freeform"])
            shape = PLFixtureFreestyle;
        
        p = [prop propertyWithKey:(shape==PLFixtureFreestyle)?@"frame":@"box"];
        box = p?p.rectValue:(shape==PLFixtureFreestyle)?NSMakeRect(0, -0, 1, 1):NSMakeRect(-0.5f, -0.5f, 1, 1);
        
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
        
        p = [prop propertyWithKey:@"density"];
        density = p?p.numberValue:1.0f;
        
        p = [prop propertyWithKey:@"groupIndex"];
        groupIndex = p?p.numberValue:0;
        
        p = [prop propertyWithKey:@"categoryBits"];
        categoryBits = p?p.numberValue:0x0001;
        
        p = [prop propertyWithKey:@"maskBits"];
        maskBits = p?p.numberValue:0xFFFF;
        
        p = [prop propertyWithKey:@"curve"];
        PLObject * obj = prop.owner;
        bezierCurve = (p&&([p type]==PLPropertyNumber)&&[obj isKindOfClass:[PLObject class]])?[obj bezierPathAtIndex:(int)[p numberValue]]:nil;
        [bezierCurve retain];
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
        case PLFixtureFreestyle:
            [file appendFormat:@"objectAddFreeform(obj,bezierCurve%u",(unsigned int)bezierCurveIndex];
            break;
        default:
            return;
    }
    
    int count = 0;
    if ((shape == PLFixtureFreestyle)&&(box.origin.x!=0 || box.origin.y!=0 || box.size.width!=1 || box.size.height!=1))
        addToken(file,[NSString stringWithFormat:@"frame = rect(%lf,%lf,%lf,%lf)",box.origin.x,box.origin.y,box.size.width,box.size.height], &count);
    
    if ((shape!=PLFixtureCircle)&&rotation)
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

-(void)encodeWithCoder:(NSCoder *)aCoder
{
    PLProperty * prop = [[[PLProperty alloc] init] autorelease];
    prop.name = @"tempProperty";
    prop.dictionaryValue = [NSDictionary dictionary];
    
    PLProperty * p;
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"shape";
    switch (shape) {
        case PLFixtureRect:
            p.stringValue = @"box";
            break;
        case PLFixtureCircle:
            p.stringValue = @"circle";
            break;
        case PLFixtureFreestyle:
            p.stringValue = @"freeform";
            break;
        default:
            p = nil;
            break;
    }
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = (shape==PLFixtureFreestyle)?@"frame":@"box";
    p.rectValue = box;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"pos";
    p.pointValue = position;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"rotation";
    p.numberValue = rotation;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"friction";
    p.numberValue = friction;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"density";
    p.numberValue = density;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"circleR";
    p.numberValue = radius;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"restitution";
    p.numberValue = restitution;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"groupIndex";
    p.numberValue = groupIndex;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"categoryBits";
    p.numberValue = categoryBits;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    p = [[[PLProperty alloc] init] autorelease];
    p.name = @"maskBits";
    p.numberValue = maskBits;
    [prop insertProperty:p atIndex:[prop childrenCount]];
    
    [aCoder encodeObject:prop forKey:@"property"];
    [aCoder encodeObject:bezierCurve forKey:@"bezierCurve"];
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [self initFromProperty:[aDecoder decodeObjectForKey:@"property"]];
    if (self)
        bezierCurve = (PLBezier*)[[aDecoder decodeObjectForKey:@"bezierCurve"] retain];
    return self;
}

-(NSString*)description
{
    switch (shape) {
        case PLFixtureCircle:
            return @"Circle";
        case PLFixtureRect:
            return @"Box";
        default:
            return @"Fixture";
    }
}

-(void)fixtureChanged
{
    [[self object] subobjectChanged:self];
    [viewController fixtureChanged];
}

-(void)setSelected:(BOOL)sel
{
    [super setSelected:sel];
    if (actor)
        actor->selectedChanged();
}

-(int)shape
{
    return shape;
}

-(NSRect)box
{
    return box;
}

-(NSPoint)position
{
    return position;
}

-(double)rotation
{
    return rotation;
}

-(double)friction
{
    return friction;
}

-(double)density
{
    return density;
}

-(double)restitution
{
    return restitution;
}

-(double)radius
{
    return radius;
}

-(int)groupIndex
{
    return groupIndex;
}

-(int)categoryBits
{
    return categoryBits;
}

-(int)maskBits
{
    return maskBits;
}

-(PLBezier*)bezierCurve
{
    return bezierCurve;
}

-(NSUndoManager*)undoManager
{
    return [(EntityController*)owner undoManager];
}

-(void)_setShape:(int)s
{
    if (shape==s) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] _setShape:shape];
    shape = s;
    [self fixtureChanged];
    [(EntityController*)owner entityDescriptionChanged:self];
}

-(void)setShape:(int)s
{
    if (shape==s) return;
    [self _setShape:s];
    [self setBezierCurve:(shape == PLFixtureFreestyle)?[[[PLBezier alloc] init] autorelease]:nil];
}

-(void)setBox:(NSRect)b
{
    if (NSEqualRects(b, box)) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setBox:box];
    box = b;
    [self fixtureChanged];
}

-(void)setPosition:(NSPoint)pos
{
    if (NSEqualPoints(pos, position)) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setPosition:position];
    position = pos;
    [self fixtureChanged];
}

-(void)setRotation:(double)rot
{
    if (rotation==rot) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setRotation:rotation];
    rotation = rot;
    [self fixtureChanged];
}

-(void)setFriction:(double)f
{
    if (friction==f) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setFriction:friction];
    friction = f;
    [self fixtureChanged];
}

-(void)setDensity:(double)d
{
    if (density==d) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setDensity:density];
    density = d;
    [self fixtureChanged];
}

-(void)setRestitution:(double)r
{
    if (r==restitution) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setRestitution:restitution];
    restitution = r;
    [self fixtureChanged];
}

-(void)setRadius:(double)r
{
    if (radius==r) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setRadius:radius];
    radius = r;
    [self fixtureChanged];
}

-(void)setGroupIndex:(int)g
{
    if (groupIndex==g) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setGroupIndex:groupIndex];
    groupIndex = g;
    [self fixtureChanged];
}

-(void)setCategoryBits:(int)c
{
    if (categoryBits==c) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setCategoryBits:categoryBits];
    categoryBits = c;
    [self fixtureChanged];
}

-(void)setMaskBits:(int)m
{
    if (maskBits==m) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setMaskBits:maskBits];
    maskBits = m;
    [self fixtureChanged];
}

-(void)setBezierCurve:(PLBezier *)b
{
    if (bezierCurve==b) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setBezierCurve:bezierCurve];
    [b retain];
    [bezierCurve release];
    bezierCurve = b;
    [self fixtureChanged];
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

-(PLObject*)object
{
    return (PLObject*)[(SubentityController*)owner object];
}

-(void)move:(NSPoint)delta
{
    if (!delta.x && !delta.y) return;
    if (shape == PLFixtureCircle)
    {
        [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setPosition:position];
        position.x+=delta.x;
        position.y+=delta.y;
        [self fixtureChanged];
    }
    if (shape == PLFixtureRect || shape == PLFixtureFreestyle)
    {
        [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setBox:box];
        box.origin.x+=delta.x;
        box.origin.y+=delta.y;
        [self fixtureChanged];
    }
}

-(void)rotate:(double)ammount
{
    if (!ammount) return;
    if (shape == PLFixtureCircle) return;
    [(PLFixture*)[[self undoManager] prepareWithInvocationTarget:self] setRotation:rotation];
    rotation+=ammount;
    [self fixtureChanged];
}

@end
