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
#import "PLImage.h"
#import "PLFixture.h"
#import "PLPrototype.h"
#import "PrototypeController.h"
#import "EntityController.h"
#import "PLObjectView.h"

@implementation PLObject
@synthesize rootProperty;
@synthesize subentityModel;
@synthesize className;
@synthesize prototype;
@synthesize actor;

-(NSUndoManager*)undoManager
{
    if ([owner isKindOfClass:[EntityController class]])
        return ((EntityController*)owner).undoManager;
    return nil;
}

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

-(void)changePrototype
{
    PLPrototype * oldPrototype = prototype;
    PLPrototype * newPrototype = [[PrototypeController singleton] prototypeForClass:className];
    
    [subentityModel disableUndo];
    [subentityModel removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [subentityModel numberOfReadOnlyEntitiesInArray:0])] fromArray:0];
    [subentityModel removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [subentityModel numberOfReadOnlyEntitiesInArray:1])] fromArray:1];
    [subentityModel insertEntities:newPrototype.images atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [newPrototype.images count])] inArray:0];
    [subentityModel insertEntities:newPrototype.fixtures atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [newPrototype.fixtures count])] inArray:1];
    [subentityModel enableUndo];
    
    prototype = newPrototype;
    [newPrototype retain];
    [oldPrototype release];
}

-(void)_removeInherited:(PLProperty*)prop :(PLProperty*)model
{
    NSMutableArray * toDelete = [[[NSMutableArray alloc] init] autorelease];
    if (prop.type == PLPropertyArray)
    {
        for (PLProperty * p in prop.arrayValue)
            if ([p isEqual:[model propertyAtIndex:p.index]])
                [toDelete addObject:p];
        [prop removeProperties:toDelete];
        for (PLProperty * p in prop.arrayValue)
            if ([p isCollection])
            {
                PLProperty * m = [model propertyAtIndex:p.index];
                if (m)
                    [self _removeInherited:p :m];
            }
    }  
    else
    if (prop.type == PLPropertyDictionary)
    {
        for (PLProperty * p in [prop.dictionaryValue allValues])
            if ([p isEqual:[model propertyWithKey:p.name]])
                [toDelete addObject:p];
        [prop removeProperties:toDelete];
        for (PLProperty * p in [prop.dictionaryValue allValues])
            if ([p isCollection])
            {
                PLProperty * m = [model propertyWithKey:p.name];
                if (m)
                    [self _removeInherited:p :m];
            }
    }
}

-(void)removeInherited
{
    [self _removeInherited:rootProperty :prototype.rootProperty];
}

-(PLProperty*)propertyAtKeyPath:(NSString*)keyPath
{
    PLProperty * p = [rootProperty propertyAtKeyPath:keyPath];
    if (!p)
        p = [prototype.rootProperty propertyAtKeyPath:keyPath];
    return p;
}

-(id)initFromLua:(lua_State*)L;
{
    self = [super initFromLua:L];
    if (self) {
        BOOL isRO = NO;
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
                lua_getfield(L, -1, "readOnly");
                if (lua_isboolean(L, -1) && lua_toboolean(L, -1))
                    isRO = YES;
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
        
        className = [[rootProperty propertyWithKey:@"class"].stringValue retain];
        prototype = [[[PrototypeController singleton] prototypeForClass:self.className] retain];
        
        subentityModel = [[SubentityController alloc] initWithObject:self];
        NSUInteger m = [prototype.images count];
        NSArray * img = [PLImage imagesFromProperty:images];
        if ([img count])
            img = [img subarrayWithRange:NSMakeRange(m, [img count]-m)];
        [subentityModel insertEntities:img atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [img count])] inArray:0];
        m = [prototype.fixtures count];
        NSArray * fxt = [PLFixture fixturesFromProperty:fixtures];
        if ([fxt count])
            fxt = [fxt subarrayWithRange:NSMakeRange(m, [fxt count]-m)];
        [subentityModel insertEntities:fxt atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [fxt count])] inArray:1];
        [images release];
        [fixtures release];
        
        [self removeInherited];
        [self changePrototype];
        
        if (isRO)
            self.readOnly = YES;
    }
    
    return self;
}

-(void)writeToFile:(NSMutableString*)file;
{
    [file appendFormat:@"obj = objectWithClass(\"%@\")\nobj.levelDes = true\n",className];
    NSUInteger i,n=[rootProperty childrenCount];
    for (i=0; i<n; i++)
    {
        PLProperty * p = [rootProperty propertyAtIndex:i];
        if ([p.name isEqual:@"class"]) continue;
        if ([p isCollection] && ![p.name isEqual:@"physics"])
            [file appendFormat:@"obj.%@ = {}\n",p.name];
        [p writeToFile:file withIndexPath:[NSString stringWithFormat:@"obj.%@",p.name]];
    }
    for (PLImage * img in [subentityModel arrayAtIndex:0])
        if (!img.readOnly)
            [img writeToFile:file];
    for (PLFixture * fxt in [subentityModel arrayAtIndex:1])
        if (!fxt.readOnly)
            [fxt writeToFile:file];
    [file appendString:@"addObject(obj)\n\n"];
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) 
    {
        rootProperty = [(PLProperty*)[aDecoder decodeObjectForKey:@"rootProperty"] retain];
        rootProperty.owner = self;
        [self markMandatory];
        
        className = [[rootProperty propertyWithKey:@"class"].stringValue retain];
        prototype = [[[PrototypeController singleton] prototypeForClass:self.className] retain];
        
        subentityModel = [[SubentityController alloc] initWithObject:self];
        NSArray * images = (NSArray *)[aDecoder decodeObjectForKey:@"images"];
        NSArray * fixtures = (NSArray *)[aDecoder decodeObjectForKey:@"fixtures"];
        [subentityModel insertEntities:images atIndex:0 inArray:0];
        [subentityModel insertEntities:fixtures atIndex:0 inArray:1];
        
        [self changePrototype];
        
    }
    return self;
}

-(void)encodeWithCoder:(NSCoder *)aCoder
{
    [aCoder encodeObject:rootProperty forKey:@"rootProperty"];
    [aCoder encodeObject:[subentityModel readWriteEntitiesInArray:0] forKey:@"images"];
    [aCoder encodeObject:[subentityModel readWriteEntitiesInArray:1] forKey:@"fixtures"];
}

-(void)objectChanged
{
    [(EntityController*)owner entityChanged:self];
    if (actor)
        actor->modelChanged();
}

-(NSString*)description
{
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
    if (p.parent==rootProperty)
    {
        if ([name isEqual:@"scripting"])
            [(EntityController*)owner entityDescriptionChanged:self]; 
        else
        if ([name isEqual:@"class"])
        {
            NSString * classN = p.stringValue;
            [classN retain];
            [className release];
            className = classN;
            [self changePrototype];
            [(EntityController*)owner entityDescriptionChanged:self]; 
        }
    }
    [self objectChanged];
}

-(void)subobjectChanged:(PLEntity*)so
{
    [self objectChanged];
}

-(void)setReadOnly:(BOOL)ro
{
    if (readOnly==ro) return;
    [super setReadOnly:ro];
    subentityModel.readOnly = ro;
    [self objectChanged];
}

-(void)move:(NSPoint)p
{
    PLProperty * prop = [rootProperty propertyWithKey:@"pos"];
    NSPoint pnt = prop.pointValue;
    pnt.x+=p.x;
    pnt.y+=p.y;
    [prop setPointValue:pnt];
}

-(void)rotate:(double)p
{
    PLProperty * prop = [rootProperty propertyWithKey:@"rotation"];
    [prop setNumberValue:prop.numberValue+p];
    
}

@end
