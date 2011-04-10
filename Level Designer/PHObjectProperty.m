//
//  PHObjectProperty.m
//  Level Designer
//
//  Created by Marius Petcu on 4/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHObjectProperty.h"


@implementation PHObjectProperty

@synthesize key;
@synthesize object;
@synthesize mandatory;

+(PHObjectProperty*)propertyWithValue:(id)v ofType:(int)ty  forKey:(NSString*)k
{
	return [[[[self class] alloc] initWithValue:v ofType:(int)ty forKey:k] autorelease];
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
	if (mandatory)
		prop = [prop initMandatoryWithValue:[[value copy] autorelease] ofType:type forKey:key];
	else
		prop = [prop initWithValue:[[value copy] autorelease] ofType:type forKey:key];
	return prop;
}

-(id)value
{
	return value;
}

-(void)setValue:(id)v
{
	if (v==value) return;
	[value release];
	switch (type) {
		case kPHObjectPropertyBool:
			value = [[NSNumber alloc] initWithBool:[(NSNumber*)v boolValue]];
			break;
		case kPHObjectPropertyNumber:
			value = [(NSNumber*)v copy];
			break;
		case kPHObjectPropertyString:
			value = [[v description] retain];
			break;
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
	[aCoder encodeObject:value forKey:@"value"];
	[aCoder encodeObject:key forKey:@"key"];
	[aCoder encodeBool:mandatory forKey:@"mandatory"];
}

-(NSString*)description
{
	return [NSString stringWithFormat:@"%@=%@",key,value];
}

-(int)type
{
	return type;
}

-(void)setType:(int)ty
{
	if (ty==kPHObjectPropertyString)
		self.stringValue = [value description];
	if (ty==kPHObjectPropertyNumber)
		self.doubleValue = [value doubleValue];
	if (ty==kPHObjectPropertyBool)
		self.boolValue = [value boolValue];
	type = ty;
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

-(NSComparisonResult)compare:(PHObjectProperty*)other
{
	return [key compare:other.key];
}

@end
