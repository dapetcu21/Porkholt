//
//  PLProperty.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/23/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLProperty.h"
#import "EntityController.h"
#import "PropertyController.h"
#import "PLObject.h"

@implementation PLProperty
@synthesize position;
@synthesize mandatory;
@synthesize parent;
@synthesize mark;

-(NSString*)stringFromLua:(lua_State*)L
{
    NSString * s = nil;
    if (lua_isnumber(L, -1))
        s = [NSString stringWithFormat:@"%d",(int)lua_tonumber(L, -1)];
    else
        if (lua_isstring(L, -1))
            s = [NSString stringWithUTF8String:lua_tostring(L, -1)];
    return s;
}

- (id)initFromLua:(lua_State *)L
{
    self = [super initFromLua:L];
    if (self) {
        type = PLPropertyNil;
        if (L)
        {
            if (lua_istable(L, -1))
            {
                lua_getfield(L, -1, "key");
                name = [[self stringFromLua:L] retain];
                lua_pop(L,1);
                
                lua_getfield(L, -1, "value");
                if (lua_isboolean(L, -1))
                    [self setBooleanValue:(BOOL)lua_toboolean(L, -1)];
                else
                if (lua_isnumber(L, -1))
                    [self setNumberValue:lua_tonumber(L, -1)];
                else
                if (lua_isstring(L, -1))
                    [self setStringValue:[NSString stringWithUTF8String:lua_tostring(L, -1)]];
                else
                if (lua_istable(L, -1))
                {
                    lua_getfield(L, -1, "array");
                    BOOL isarray = (lua_isboolean(L, -1) && lua_toboolean(L, -1));
                    lua_pop(L,1);
                    
                    lua_getfield(L, -1, "n");
                    int n = lua_isnumber(L, -1)?lua_tonumber(L, -1):0;
                    lua_pop(L,1);
                    
                    NSMutableArray * arry = [[[NSMutableArray alloc] initWithCapacity:n] autorelease];
                    NSMutableDictionary * dict = isarray?nil:[[[NSMutableDictionary alloc]initWithCapacity:n] autorelease];
                    
                    for (int i=0; i<n; i++)
                    {
                        lua_pushnumber(L, i);
                        lua_gettable(L, -2);
                        if (lua_istable(L, -1))
                        {
                            PLProperty * prop = [[[PLProperty alloc] initFromLua:L] autorelease];
                            if (prop.type == PLPropertyDictionary)
                            {
                                BOOL origin = ([[prop propertyWithKey:@"x"] type]==PLPropertyNumber)&&([[prop propertyWithKey:@"y"] type]==PLPropertyNumber);
                                
                                BOOL size = ([[prop propertyWithKey:@"width"] type]==PLPropertyNumber)&&([[prop propertyWithKey:@"height"] type]==PLPropertyNumber);
                                
                                if (origin&&([prop childrenCount]==2))
                                    [prop setType:PLPropertyPoint];
                                else
                                if (origin&&size&&([prop childrenCount]==4))
                                    [prop setType:PLPropertyRect];
                            }
                            if (isarray)
                                [arry addObject:prop];
                            else
                            {
                                [arry addObject:prop.name];
                                [dict setObject:prop forKey:prop.name];
                            }
                        }
                        lua_pop(L,1);
                    }
                    
                    if (isarray)
                        [self setRetainedArrayValue:arry];
                    else
                        [self setRetainedDictionaryValue:dict retainedOrder:arry];
                }
                lua_pop(L,1);
            }
        } else 
            name = [@"untitled" retain];
    }
    
    return self;
}


-(void)_setNil
{
    switch (type)
    {
        case PLPropertyArray:
            for (PLProperty * p in value.array)
                p->parent = nil;
            [value.array release];
            break;
        case PLPropertyDictionary:
            for (PLProperty * p in [value.dictionary.value allValues])
                p->parent = nil;
            [value.dictionary.orderedKeys release];
            [value.dictionary.value release];
            break;
        case PLPropertyString:
            [value.string release];
            break;
    }
    type = PLPropertyNil;
}


-(void)dealloc{
    [self _setNil];
    [name release];
    [super dealloc];
}

-(NSUndoManager*)undoManager
{
    return ((EntityController*)((PLEntity*)owner).owner).undoManager;
}

-(void)setController:(PropertyController *)c
{
    controller = c;
    if (type == PLPropertyDictionary)
        for (PLProperty * p in [value.dictionary.value allValues])
            p.controller = c;
    else
    if (type == PLPropertyArray)
        for (PLProperty * p in value.array)
            p.controller = c;
}

-(void)setOwner:(id)o
{
    
    owner = o;
    if (type == PLPropertyDictionary)
        for (PLProperty * p in [value.dictionary.value allValues])
            p.owner = o;
    else
    if (type == PLPropertyArray)
        for (PLProperty * p in value.array)
            p.owner = o;
}

-(PropertyController*)controller
{
    return controller;
}

#pragma mark Manipulation

-(void)propertyChanged
{
    [controller propertyChanged:self];
    [(PLObject*)owner propertyChanged:self];
}

-(NSString*)name
{
    return name;
}

-(void)setName:(NSString *)nm
{
    if ([nm isEqualToString:name]) return;
    if (parent && parent.type == PLPropertyDictionary)
        [parent renameChild:self to:nm];
    else
    {
        [(PLProperty*)[self.undoManager prepareWithInvocationTarget:self] setName:name];
        [nm retain];
        [name release];
        name = nm;
    }
}

-(NSIndexSet*)indexesForProperties:(NSArray*)props
{
	NSMutableIndexSet * is = [NSMutableIndexSet indexSet];
	for (PLProperty * prop in props)
		[is addIndex:prop.index];
	return is;
}

-(NSArray*)propertiesForIndexes:(NSIndexSet*)indexes
{
	NSMutableArray * a = [NSMutableArray arrayWithCapacity:[indexes count]];
	[indexes enumerateIndexesUsingBlock: ^(NSUInteger idx, BOOL *stop) {
		[a addObject:[self propertyAtIndex:idx]];
	}];
	return a;
}

-(void)insertProperties:(NSArray*)props atIndexes:(NSIndexSet*)indexes
{
	NSUInteger n = [props count];
	if (!n || n!=[indexes count]) return;
	[(PLProperty*)[self.undoManager prepareWithInvocationTarget:self] removePropertiesAtIndexes:indexes];
	if (type==PLPropertyDictionary)
	{
		NSMutableArray * keys = [NSMutableArray arrayWithCapacity:n];
        for(int i=0; i<n; i++)
        {
            PLProperty * p = (PLProperty*)[props objectAtIndex:i];
            NSString * nm = [self proposedKeyForName:p.name withAdditions:keys];
            p.name = nm;
            [keys addObject:nm];
        }
		[value.dictionary.value	setValuesForKeysWithDictionary:[NSDictionary dictionaryWithObjects:props forKeys:keys]];
        [value.dictionary.orderedKeys insertObjects:keys atIndexes:indexes];
    }
    else
    if (type==PLPropertyArray)
    {
        [value.array insertObjects:props atIndexes:indexes];
    }
    [self rebuildArrayNames];
    [self propertyChanged];
}

-(void)insertProperty:(PLProperty*)p atIndex:(NSUInteger)idx
{
	if (p==nil) return;
	[self insertProperties:[NSArray arrayWithObject:p] atIndexes:[NSIndexSet indexSetWithIndex:idx]];
}

-(void)removeProperties:(NSArray*)props
{
	[self removeProperties:props atIndexes:[self indexesForProperties:props]];
}

-(void)removePropertiesAtIndexes:(NSIndexSet*)indexes
{
	[self removeProperties:[self propertiesForIndexes:indexes] atIndexes:indexes];
}

-(void)removeProperty:(PLProperty*)p
{
	if (p==nil) return;
	[self removeProperties:[NSArray arrayWithObject:p]];
}

-(void)removeProperties:(NSArray*)props atIndexes:(NSIndexSet*)indexes; 
{
	NSUInteger n = [props count];
	if (!n || n!=[indexes count]) return;
	[(PLProperty*)[self.undoManager prepareWithInvocationTarget:self] insertProperties:props atIndexes:indexes];
    for (PLProperty * p in props)
    {
        p.owner = nil;
        p.controller = nil;
    }
	if (type==PLPropertyDictionary)
	{
		[value.dictionary.value removeObjectsForKeys:[self keysForArray:props]];
        [value.dictionary.orderedKeys removeObjectsAtIndexes:indexes];
    }
    else
    if (type==PLPropertyArray)
        [value.array removeObjectsAtIndexes:indexes];
    [self rebuildArrayNames];
    [self propertyChanged];
}


#pragma mark Values and type casting

-(int)type
{
    return type;
}

-(void)setType:(int)t
{
    if (type==t) return;
    switch (t) {
        case PLPropertyString:
            self.stringValue = self.stringValue;
            break;
        case PLPropertyNumber:
            self.numberValue = self.numberValue;
            break;
        case PLPropertyBoolean:
            self.booleanValue = self. booleanValue;
            break;
        case PLPropertyArray:
            [self setRetainedArrayValue:(NSMutableArray*)self.arrayValue];
            break;
        case PLPropertyDictionary:
            [self setRetainedDictionaryValue:(NSMutableDictionary*)self.dictionaryValue retainedOrder:(NSMutableArray*)self.dictionaryOrder];
            break;
        case PLPropertyPoint:
            self.pointValue = self.pointValue;
            break;
        case PLPropertyRect:
            self.rectValue = self.rectValue;
            break;
        default:
            [self setNil];
            break;
    }
}

-(void)pushOnTheUndoStack
{
    NSUndoManager * undoMan = self.undoManager;
    switch (type) {
        case PLPropertyString:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setStringValue:value.string];
            break;
        case PLPropertyNumber:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setNumberValue:value.number];
            break;
        case PLPropertyBoolean:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setBooleanValue:value.boolean];
            break;
        case PLPropertyPoint:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setPointValue:value.point];
            break;
        case PLPropertyRect:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setRectValue:value.rect];
            break;
        case PLPropertyArray:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setRetainedArrayValue:value.array];
            break;
        case PLPropertyDictionary:
            [(PLProperty*)[undoMan prepareWithInvocationTarget:self] setRetainedDictionaryValue:value.dictionary.value retainedOrder:value.dictionary.orderedKeys];
            break;
    }
}

-(void)setNil
{  
    if (type==PLPropertyNil) return;
    [self pushOnTheUndoStack];
    [self _setNil];
    [self propertyChanged];
}

-(void)setNumberValue:(double)v
{
    if (type==PLPropertyNumber && value.number == v) return;
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyNumber;
    value.number = v;
    [self propertyChanged];
}

-(void)setBooleanValue:(BOOL)v
{
    if (type==PLPropertyBoolean && value.boolean == v) return;
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyBoolean;
    value.boolean = v;
    [self propertyChanged];
}

-(void)setStringValue:(NSString*)str
{
    if (type==PLPropertyString && [str isEqual:value.string]) return;
    [self pushOnTheUndoStack];
    [str retain];
    [self _setNil];
    type = PLPropertyString;
    value.string = str;
    [self propertyChanged];
}

-(void)rebuildArrayNames
{
    if (type == PLPropertyArray)
    {
        int i=0;
        for (PLProperty * p in value.array)
        {
            p.index = i;
            p.owner = owner;
            p.controller = controller;
            p->parent = self;
            [p setName:[NSString stringWithFormat:@"%d",i]];
            i++;
        }
    } else
    if (type == PLPropertyDictionary) 
    {
        NSUInteger n = [value.dictionary.orderedKeys count];
        for (NSUInteger i=0; i<n; i++)
        {
            PLProperty * p = (PLProperty*)[value.dictionary.value objectForKey:[value.dictionary.orderedKeys objectAtIndex:i]];
            p.index = i;
            p.owner = owner;
            p.controller = controller;
            p->parent = self;
        }
    }
}

-(void)setRetainedArrayValue:(NSMutableArray*)a
{
    [self pushOnTheUndoStack];
    [a retain];
    [self _setNil];
    type = PLPropertyArray;
    value.array = a;
    [self rebuildArrayNames];
    [self propertyChanged];
}

-(void)setArrayValue:(NSArray*)a
{
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyArray;
    value.array = [[NSMutableArray alloc] initWithArray:a];
    [self rebuildArrayNames];
    [self propertyChanged];
}

-(void)setDictionaryValue:(NSDictionary*)dictionary
{
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyDictionary;
    value.dictionary.value = [[NSMutableDictionary alloc] initWithDictionary:dictionary];
    value.dictionary.orderedKeys = [[NSMutableArray alloc] initWithArray:[value.dictionary.value allKeys]];
    [self rebuildArrayNames];
    [self propertyChanged];
}

-(void)setDictionaryValue:(NSDictionary*)dictionary order:(NSArray *)order
{
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyDictionary;
    value.dictionary.value = [[NSMutableDictionary alloc] initWithDictionary:dictionary];
    value.dictionary.orderedKeys = [[NSMutableArray alloc] initWithArray:order];
    [self rebuildArrayNames];
    [self propertyChanged];
}

-(void)setRetainedDictionaryValue:(NSMutableDictionary *)dictionaryValue retainedOrder:(NSMutableArray *)order
{
    [self pushOnTheUndoStack];
    [dictionaryValue retain];
    [order retain];
    [self _setNil];
    type = PLPropertyDictionary;
    value.dictionary.value = dictionaryValue;
    value.dictionary.orderedKeys = order;
    [self rebuildArrayNames];
    [self propertyChanged];
}

-(void)setPointValue:(NSPoint)pnt
{
    if (type==PLPropertyPoint && value.point.x == pnt.x && value.point.y == pnt.y) return;
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyPoint;
    value.point = pnt;
    [self propertyChanged];
}

-(void)setRectValue:(NSRect)rct
{
    if (type==PLPropertyRect && NSEqualRects(value.rect, rct)) return;
    [self pushOnTheUndoStack];
    [self _setNil];
    type = PLPropertyRect;
    value.rect = rct;
    [self propertyChanged];
}


-(void)setPointValueFromString:(NSString*)s
{
    double x,y;
    sscanf([s UTF8String],"x:%lly:%lf",&x,&y);
    [self setPointValue:NSMakePoint(x, y)];
}

-(void)setRectValueFromString:(NSString*)s
{
    double x,y,w,h;
    sscanf([s UTF8String],"x:%lly:%lfw:%lfh:%lf",&x,&y,&w,&h);
    [self setRectValue:NSMakeRect(x, y, w, h)];
}

-(void)convertToString
{
    [self setType:PLPropertyString];
}

-(void)convertToNumber
{
    [self setType:PLPropertyNumber];
}

-(void)convertToBoolean
{
    [self setType:PLPropertyBoolean];
}

-(void)convertToArray
{
    [self setType:PLPropertyArray];
}

-(void)convertToDictionary
{
    [self setType:PLPropertyDictionary];
}

-(void)convertToPoint
{
    [self setType:PLPropertyPoint];
}

-(void)convertToRect
{
    [self setType:PLPropertyRect];
}


-(double)numberValue
{
    switch (type) {
        case PLPropertyNumber:
            return value.number;
        case PLPropertyBoolean:
            return value.boolean?1:0;
        case PLPropertyString:
            return [value.string doubleValue];
        case PLPropertyPoint:
            return value.point.x;
        case PLPropertyRect:
            return value.rect.origin.x;
        default:
            return 0;
    }
}

-(BOOL)booleanValue
{
    switch (type) {
        case PLPropertyBoolean:
            return value.boolean;
        case PLPropertyNumber:
            return value.number!=0;
        case PLPropertyString:
            return [value.string boolValue];
        default:
            return NO;
    }
}

-(NSString*)stringValue
{
    switch (type) {
        case PLPropertyString:
            return value.string;
        case PLPropertyBoolean:
            return value.boolean?@"Yes":@"No";
        case PLPropertyNumber:
            return [NSString stringWithFormat:@"%.6f",value.number];
        case PLPropertyPoint:
            return [NSString stringWithFormat:@"x:%.3f y:%.3f",value.point.x,value.point.y];
        case PLPropertyRect:
            return [NSString stringWithFormat:@"x:%.3f y:%.3f w:%.3f h:%.3f",value.rect.origin.x,value.rect.origin.y,value.rect.size.width,value.rect.size.height];
        default:
            return @"";
    }
}

-(NSMutableArray*)keysForArray:(NSArray*)arr
{
    NSMutableArray * a = [NSMutableArray arrayWithCapacity:[arr count]];
    for (PLProperty * p in arr)
        [a addObject:p.name];
    return a;
}

-(NSDictionary*)dictionaryValue
{
    switch (type) {
        case PLPropertyDictionary:
            return value.dictionary.value;
        case PLPropertyArray:
            return [NSMutableDictionary dictionaryWithObjects:value.array forKeys:[self keysForArray:value.array]];
        default:
            return [NSMutableDictionary dictionary];
    }
}

-(NSArray*)dictionaryOrder
{
    switch (type) {
        case PLPropertyDictionary:
            return value.dictionary.orderedKeys;
        case PLPropertyArray:
            return [self keysForArray:value.array];
        default:
            return [NSMutableArray array];
    }
}

-(NSArray*)arrayValue
{
    switch (type) {
        case PLPropertyArray:
            return value.array;
        case PLPropertyDictionary:
            return [NSMutableArray arrayWithArray:[value.dictionary.value allValues]];
        default:
            return [NSMutableArray array];
    }
}

-(NSPoint)pointValue
{
    switch (type) {
        case PLPropertyPoint:
            return value.point;
        case PLPropertyRect:
            return value.rect.origin;
        case PLPropertyNumber:
            return NSMakePoint(value.number, value.number);
        case PLPropertyDictionary:
            return NSMakePoint([self propertyWithKey:@"x"].numberValue,
                               [self propertyWithKey:@"y"].numberValue);
        default:
            return NSMakePoint(0, 0);
    }
}

-(NSRect)rectValue
{
    switch (type) {
        case PLPropertyRect:
            return value.rect;
        case PLPropertyPoint:
            return NSMakeRect(value.point.x, value.point.y, 0, 0);
        case PLPropertyNumber:
            return NSMakeRect(0, 0, value.number, value.number);
        case PLPropertyDictionary:
            return NSMakeRect([self propertyWithKey:@"x"].numberValue,
                              [self propertyWithKey:@"y"].numberValue,
                              [self propertyWithKey:@"width"].numberValue,
                              [self propertyWithKey:@"height"].numberValue);
        default:
            return NSMakeRect(0, 0, 0, 0);
    }
}

-(PLProperty*)propertyWithKey:(NSString *)key
{
    switch (type) {
        case PLPropertyDictionary:
            return [value.dictionary.value objectForKey:key];
        case PLPropertyArray:
            return [value.array objectAtIndex:[key intValue]];
        default:
            return nil;
    }
}

-(PLProperty*)propertyAtIndex:(NSUInteger)indx
{
    switch (type) {
        case PLPropertyArray:
            return [value.array objectAtIndex:indx];
        case PLPropertyDictionary:
            return [value.dictionary.value objectForKey:[value.dictionary.orderedKeys objectAtIndex:indx]];
        default:
            return nil;
    }
}

-(PLProperty*)propertyWithKeyPath:(NSString *)path
{
    NSUInteger idx = [path rangeOfString:@"."].location;
    if (idx>=[path length]) 
        return [self propertyWithKey:path];
    return [[self propertyWithKey:[path substringToIndex:idx]] propertyWithKeyPath:[path substringFromIndex:idx+1]];
}

-(NSString*)proposedKeyForName:(NSString*)nm
{
    if (type!=PLPropertyDictionary) 
        return nm;
    while ([value.dictionary.value objectForKey:nm])
        nm = [nm stringByAppendingString:@"_"];
    return nm;
}

-(NSString*)proposedKeyForName:(NSString*)nm withAdditions:(NSArray *)add
{
    if (type!=PLPropertyDictionary) 
        return nm;
    NSSet * d = [NSSet setWithArray:add];
     while ([value.dictionary.value objectForKey:nm]||[d containsObject:nm])
         nm = [nm stringByAppendingString:@"_"];
    return nm;
}


-(void)renameChild:(PLProperty *)child to:(NSString *)newName
{
    if (type!=PLPropertyDictionary)
        return;
    if ([newName isEqualToString:child.name])
        return;
    newName = [self proposedKeyForName:newName];
    [(PLProperty*)[self.undoManager prepareWithInvocationTarget:self] renameChild:child to:child.name];
    [child retain];
    [value.dictionary.value removeObjectForKey:child.name];
    [value.dictionary.value setValue:child forKey:newName];
    [value.dictionary.orderedKeys replaceObjectAtIndex:child.index withObject:newName];
    [newName retain];
    [child->name release];
    child->name = newName;
    [self propertyChanged];
}

-(NSUInteger)childrenCount
{
    return (type==PLPropertyDictionary)?[value.dictionary.value count]:(type==PLPropertyArray)?[value.array count]:0;
}

-(BOOL)isCollection
{
    return (type==PLPropertyArray)||(type==PLPropertyDictionary);
}

@end
