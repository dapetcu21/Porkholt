//
//  PHObjectProperty.m
//  Level Designer
//
//  Created by Marius Petcu on 4/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHObjectProperty.h"
#import "PHObject.h"
#import "ObjectController.h"

@implementation PHObjectProperty

@synthesize key;
@synthesize mandatory;
@synthesize userData;

+(PHObjectProperty*)propertyWithValue:(id)v ofType:(int)ty  forKey:(NSString*)k
{
	return [[[[self class] alloc] initWithValue:v ofType:(int)ty forKey:k] autorelease];
}

-(void)loadFromLua:(lua_State*)L
{
    int n = 0;
    lua_pushstring(L, "n");
    lua_gettable(L, -2);
    if (lua_isnumber(L, -1))
        n = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    bool array = false;
    lua_pushstring(L,"array");
    lua_gettable(L,-2);
    if (lua_isboolean(L,-1))
        array = lua_toboolean(L,-1);
    lua_pop(L,1);
    
    if (array)
        type = kPHObjectPropertyArray;
    
    NSMutableArray * properties = [self mutableChildNodes];
    
    for (int i=0; i<n; i++)
    {
        lua_pushnumber(L, i);
        lua_gettable(L, -2);
        if (lua_istable(L,-1))
        {
            id _value = nil;
            NSString * _key = nil;
            BOOL inherited = NO;
            BOOL tree = NO;
            int _type;
            
            lua_pushstring(L, "inherited");
            lua_gettable(L, -2);
            if (lua_isboolean(L,-1))
                inherited = lua_toboolean(L, -1);
            lua_pop(L,1);
            
            lua_pushstring(L, "key");
            lua_gettable(L, -2);
            if (lua_isstring(L, -1))
                _key = [NSString stringWithUTF8String:lua_tostring(L,-1)];
            lua_pop(L,1);
            
            lua_pushstring(L, "value");
            lua_gettable(L, -2);
            if (lua_isboolean(L, -1))
            {
                _value = [NSNumber numberWithBool:lua_toboolean(L, -1)];
                _type = kPHObjectPropertyBool;
            } else
            if (lua_isnumber(L, -1))
            {
                _value = [NSNumber numberWithDouble:lua_tonumber(L, -1)];
                _type = kPHObjectPropertyNumber;
            } else
            if (lua_isstring(L, -1))
            {
                _value = [NSString stringWithUTF8String:lua_tostring(L, -1)];
                _type = kPHObjectPropertyString;
            } else
            if (lua_istable(L, -1))
            {
                tree=YES;
                _value = nil;
                _type = kPHObjectPropertyTree;
            }
        
            if (_key&&(_value||tree))
            {
                PHObjectProperty * prop = [PHObjectProperty propertyWithValue:_value ofType:_type forKey:_key];
                prop.parentObject = self.parentObject;
                
                if (!inherited)
                    [properties addObject:prop];
                if (tree)
                    [prop loadFromLua:L];
            }
            lua_pop(L,1);
        }
        lua_pop(L, 1);
    }
    if (type!=kPHObjectPropertyArray)
        [properties sortUsingSelector:@selector(compare:)];
}

-(id)initWithValue:(id)v ofType:(int)ty  forKey:(NSString*)k
{
	if (self= [super init])
	{
		type = ty;
		self.value = v;
		self.key = k;
	}
	return self;
}

-(id)initMandatoryWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;
{
	if (self= [super init])
	{
		type = ty;
		self.value = v;
		self.key = k;
		mandatory = TRUE;
	}
	return self;
}
+(PHObjectProperty*)mandatoryPropertyWithValue:(id)v ofType:(int)ty  forKey:(NSString*)k;
{
	return [[[[self class] alloc] initMandatoryWithValue:v ofType:(int)ty forKey:k] autorelease];
}

-(void)dealloc
{
	[value release];
	[key release];
	[super dealloc];
}

-(id)copyWithZone:(NSZone *)zone
{
	PHObjectProperty * prop = [PHObjectProperty alloc];
    id _value = (type==kPHObjectPropertyTree || type==kPHObjectPropertyArray)?nil:[[value copy] autorelease];
	if (mandatory)
		prop = [prop initMandatoryWithValue:_value ofType:type forKey:key];
	else
		prop = [prop initWithValue:_value ofType:type forKey:key];
    NSMutableArray * arr = [prop mutableChildNodes];
    for (PHObjectProperty * pr in [self childNodes])
    {
        [arr addObject:[[pr copy] autorelease]];
    }
	return prop;
}

-(void)setUndoable:(NSUndoManager*)man key:(NSString*)nkey
{
    [[man prepareWithInvocationTarget:self] setUndoable:man key:self.key];
    [self setKey:nkey];
}

-(void)setUndoable:(NSUndoManager*)man doubleValue:(double)val
{
    [[man prepareWithInvocationTarget:self] setUndoable:man value:self.value];
    [self setDoubleValue:val];
}

-(void)setUndoable:(NSUndoManager*)man boolValue:(BOOL)val
{
    [[man prepareWithInvocationTarget:self] setUndoable:man value:self.value];
    [self setBoolValue:val];
}

-(void)setUndoable:(NSUndoManager*)man stringValue:(NSString*)val
{
    [[man prepareWithInvocationTarget:self] setUndoable:man value:self.value];
    [self setStringValue:val];
}
-(void)setUndoable:(NSUndoManager*)man value:(id)val
{
    [[man prepareWithInvocationTarget:self] setUndoable:man value:self.value];
    [self setValue:val];
}

-(id)value
{
    if (type==kPHObjectPropertyTree || type == kPHObjectPropertyArray)
        return [[[self childNodes] copy] autorelease];
	return value;
}

-(void)setValue:(id)v
{
	if (v==value) return;
	[value release];
    if (type!=kPHObjectPropertyTree && type !=kPHObjectPropertyArray)
    {
        if (!v || [v isKindOfClass: [NSArray class]])
            v = @"";
    }
	switch (type) {
		case kPHObjectPropertyBool:
			value = [[NSNumber alloc] initWithBool:[(NSNumber*)v boolValue]];
			break;
		case kPHObjectPropertyNumber:
			value = [[NSNumber alloc] initWithDouble:[(NSNumber*)v doubleValue]];
			break;
		case kPHObjectPropertyString:
			value = [[v description] retain];
			break;
        case kPHObjectPropertyArray:
        case kPHObjectPropertyTree:
        {
            NSMutableArray * arr = [self mutableChildNodes];
            [arr removeAllObjects];
            if (v && [v isKindOfClass:[NSArray class]])
                [arr addObjectsFromArray:(NSArray*)v];
            value = nil;
            break;
        }
	}
}

-(void)setDoubleValue:(double)v
{
	if (type!=kPHObjectPropertyNumber) return;
	[value release];
	value = [[NSNumber alloc] initWithDouble:v];
}

-(void)setIntValue:(int)v
{
	if (type!=kPHObjectPropertyNumber) return;
	[value release];
	value = [[NSNumber alloc] initWithInt:v];
}

-(void)setBoolValue:(BOOL)v
{
	if (type!=kPHObjectPropertyNumber) return;
	[value release];
	value = [[NSNumber alloc] initWithBool:v];
}

-(void)setStringValue:(NSString*)v
{
	if (type==kPHObjectPropertyString)
	{
		[v retain];
		[value release];
		value = v;
		return;
	}
	if (type==kPHObjectPropertyNumber)
	{
		NSNumber * nr = [[NSNumber alloc] initWithDouble:[v doubleValue]];
		[value release];
		value = nr;
	}
	if (type==kPHObjectPropertyBool)
	{
		NSNumber * nr = [[NSNumber alloc] initWithBool:[v boolValue]];
		[value release];
		value = nr;
	}
}

-(double)doubleValue
{
	return [value doubleValue];
}
-(int)intValue
{
	return [value intValue];
}
-(BOOL)boolValue
{
	return [value boolValue];
}
-(NSString*)stringValue
{
	return [value description];
}

-(id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init])
	{
		type = [aDecoder decodeIntForKey:@"type"];
		self.value = [aDecoder decodeObjectForKey:@"value"];
		self.key = [aDecoder decodeObjectForKey:@"key"];
		mandatory = [aDecoder decodeBoolForKey:@"mandatory"];
	}
	return self;
}

-(void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeInt:type forKey:@"type"];
	[aCoder encodeObject:self.value forKey:@"value"];
	[aCoder encodeObject:self.key forKey:@"key"];
	[aCoder encodeBool:mandatory forKey:@"mandatory"];
    
}

-(NSString*)description
{
	return [NSString stringWithFormat:@"%@=%@",self.key,self.value];
}

-(int)type
{
	return type;
}

-(void)setUndoable:(NSUndoManager*)man type:(int)_type andValue:(id)_val
{
    [[man prepareWithInvocationTarget:self] setUndoable:man type:self.type andValue:self.value];
    [self setType:_type];
    [self setValue:_val];
}

-(void)fixArrayKeysUndoable:(NSUndoManager*)man
{
    int i=0;
    NSArray * arr = [self childNodes];
    int n=[arr count];
    [man beginUndoGrouping];
    for (i=0; i<n; i++)
    {
        PHObjectProperty * prop = (PHObjectProperty*)[arr objectAtIndex:i];
        [prop setUndoable:man key:[NSString stringWithFormat:@"%d",i]];
    }
    [man endUndoGrouping];
}

-(void)setType:(int)ty
{
    BOOL wasCollection = (type==kPHObjectPropertyTree || type==kPHObjectPropertyArray);
    BOOL isCollection = (ty==kPHObjectPropertyTree || ty==kPHObjectPropertyArray);
    if (wasCollection)
    {   
        if (!isCollection)
        {
            [[self mutableChildNodes] removeAllObjects];
            [value release];
            value = [@"" retain];
        }
    };
    type = ty;
	if (ty==kPHObjectPropertyString)
		self.stringValue = [value description];
	if (ty==kPHObjectPropertyNumber)
		self.doubleValue = [value doubleValue];
	if (ty==kPHObjectPropertyBool)
		self.boolValue = [value boolValue];
    if (!wasCollection && isCollection)
        self.value = nil;
}

-(void)convertToString
{
	self.type = kPHObjectPropertyString;
}

-(void)convertToNumber
{
	self.type = kPHObjectPropertyNumber;
}

-(void)convertToBool
{
	self.type = kPHObjectPropertyBool;
}

-(void)convertToTree
{
    self.type = kPHObjectPropertyTree;
}

-(void)convertToArray
{
    self.type = kPHObjectPropertyArray;
}

-(NSComparisonResult)compare:(PHObjectProperty*)other
{
	return [key compare:other.key];
}

-(PHObject*)parentObject
{
    return parentObject;
    return nil;
}

-(void)setParentObject:(PHObject*)obj
{
    parentObject = obj;
    for (PHObjectProperty * prop in [self childNodes])
        prop.parentObject = obj;
}

-(PHObjectProperty*)propertyForKey:(NSString*)_key
{
    if (type!=kPHObjectPropertyTree) 
        return nil;
    NSArray * arr = [self childNodes];
    for (PHObjectProperty* prop in arr)
        if ([prop.key isEqualToString:_key])
            return prop;
    return nil;
}

-(PHObjectProperty*)propertyAtIndex:(NSUInteger)index
{
    if (type!=kPHObjectPropertyArray) 
        return nil;
    NSArray * arr = [self childNodes];
    return [arr objectAtIndex:index];
}

-(BOOL)selected
{
    return selected;
}

-(void)setSelected:(BOOL)sel
{
    if (sel==selected) return;
    [parentObject.controller setProperty:self selected:sel];
}

-(void)updateSelected:(BOOL)sel
{
    selected = sel;
//    NSLog(@"%@ setSelected:%d",self,sel);
}

@end
