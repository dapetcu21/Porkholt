//
//  PHObject.m
//  Level Designer
//
//  Created by Marius Petcu on 4/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHObject.h"
#import "PHObjectProperty.h"
#import "ObjectView.h"

@implementation PHObject

@synthesize classProperty;
@synthesize posXProperty;
@synthesize posYProperty;
@synthesize readOnly;
@synthesize rotationProperty;
@synthesize view;

-(id)init
{
	if (self = [super init])
	{
		properties = [[NSMutableArray alloc] init];
		[properties addObject:classProperty=[PHObjectProperty mandatoryPropertyWithValue:@"PHObject" ofType:kPHObjectPropertyString forKey:@"class"]];
		[properties addObject:posXProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"posX"]];
		[properties addObject:posYProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"posY"]];
		[properties addObject:rotationProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"rotation"]];
	}
	return self;
}

-(id)initFromLua:(lua_State*)L
{
	if (self = [super init])
	{
		properties = [[NSMutableArray alloc] init];
		
		int n = 0;
		lua_pushstring(L, "n");
		lua_gettable(L, -2);
		if (lua_isnumber(L, -1))
			n = lua_tonumber(L, -1);
		lua_pop(L,1);
		
		lua_pushstring(L, "readOnly");
		lua_gettable(L, -2);
		readOnly = YES;
		if (lua_isboolean(L, -1))
			readOnly = lua_toboolean(L, -1);
		lua_pop(L, 1);
		
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L,-1))
			{
				id value = nil;
				NSString * key = nil;
				BOOL inherited = NO;
				int type;
				
				lua_pushstring(L, "inherited");
				lua_gettable(L, -2);
				if (lua_isboolean(L,-1))
					inherited = lua_toboolean(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "key");
				lua_gettable(L, -2);
				if (lua_isstring(L, -1))
					key = [NSString stringWithUTF8String:lua_tostring(L,-1)];
				lua_pop(L,1);
				
				lua_pushstring(L, "value");
				lua_gettable(L, -2);
				if (lua_isboolean(L, -1))
				{
					value = [NSNumber numberWithBool:lua_toboolean(L, -1)];
					type = kPHObjectPropertyBool;
				} else
				if (lua_isnumber(L, -1))
				{
					value = [NSNumber numberWithDouble:lua_tonumber(L, -1)];
					type = kPHObjectPropertyNumber;
				} else
				if (lua_isstring(L, -1))
				{
					value = [NSString stringWithUTF8String:lua_tostring(L, -1)];
					type = kPHObjectPropertyString;
				}
				lua_pop(L,1);
				
				if (key&&value)
				{
					BOOL mandatory = [key isEqualToString:@"class"]||
					[key isEqualToString:@"posX"]||
					[key isEqualToString:@"posY"]||
					[key isEqualToString:@"rotation"];
					PHObjectProperty * prop;
					if (mandatory)
						prop = [PHObjectProperty mandatoryPropertyWithValue:value ofType:type forKey:key];
					else
						prop = [PHObjectProperty propertyWithValue:value ofType:type forKey:key];
					
					if ([key isEqualToString:@"class"])
						classProperty = prop;
					if ([key isEqualToString:@"posX"])
						posXProperty = prop;
					if ([key isEqualToString:@"posY"])
						posYProperty = prop;
					if ([key isEqualToString:@"rotation"])
						rotationProperty = prop;
					
					if (!inherited)
						[properties addObject:prop];
				}
				
			}
			lua_pop(L, 1);
		}
		[properties sortUsingSelector:@selector(compare:)];
	}
	return self;
}

-(void)saveToFile:(NSMutableString*)file
{
	[file appendFormat:@"\nobj = objectWithClass(\"%@\");\n",self.className];
	for (PHObjectProperty * prop in properties)
	{
		if ([prop.key isEqual:@"class"]) continue;
		[file appendFormat:@"obj.%@ = ",prop.key];
		switch (prop.type) {
			case kPHObjectPropertyBool:
				[file appendFormat:@"%s;\n",prop.boolValue?"true":"false"];
				break;
			case kPHObjectPropertyString:
				[file appendFormat:@"[[%@]];\n",prop.stringValue];
				break;
			case kPHObjectPropertyNumber:
				[file appendFormat:@"%lf;\n",prop.doubleValue];
				break;
		}
	}
	[file appendFormat:@"obj.levelDes = true;\naddObject(obj);\n"];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init])
	{		
		[self setProperties:[[aDecoder decodeObjectForKey:@"properties"] retain]];
		for (PHObjectProperty * property in properties)
		{
			if ([property.key isEqualToString:@"class"])
				classProperty = property;
			if ([property.key isEqualToString:@"posX"])
				posXProperty = property;
			if ([property.key isEqualToString:@"posY"])
				posYProperty = property;
			if ([property.key isEqualToString:@"rotation"])
				rotationProperty = property;
		}
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)encoder
{
	[encoder encodeObject:properties forKey:@"properties"];
}

-(void)setProperties:(NSArray*)obj
{
	if (properties==obj) return;
	[properties retain];
	properties = [[NSMutableArray alloc] initWithArray:obj];
}

-(NSMutableArray*)properties
{
	return properties;
}

-(void)dealloc
{
	[super dealloc];
}

-(void)setClassName:(NSString *)cn
{
	[classProperty setValue:cn];
}

-(NSString*)className
{
	return classProperty.value;
}

-(NSPoint)position
{
	NSPoint pos;
	pos.x=posXProperty.doubleValue;
	pos.y=posYProperty.doubleValue;
	return pos;
}

-(void)setPosition:(NSPoint)pos
{
	posXProperty.doubleValue = pos.x;
	posYProperty.doubleValue = pos.y;
	[self positionChanged];
}

-(void)move:(NSPoint)mv
{
	posXProperty.doubleValue = posXProperty.doubleValue+mv.x;
	posYProperty.doubleValue = posYProperty.doubleValue+mv.y;
	[self positionChanged];
}

-(double)rotation
{
	return rotationProperty.doubleValue;
}

-(void)setRotation:(double)rot
{
	rotationProperty.doubleValue = rot;
	[self positionChanged];
}

-(void)positionChanged
{
	if (!view) return;
	NSSize size = view.frame.size;
	[view setFrameOrigin:NSMakePoint(posXProperty.doubleValue-size.width/2, posYProperty.doubleValue-size.height/2)];
	view.rotation = rotationProperty.doubleValue;
}

-(BOOL)editable
{
	return !readOnly;
}

-(void)setEditable:(BOOL)sa
{
	readOnly=!sa;
}

-(id)copyWithZone:(NSZone *)zone
{
	PHObject * obj = [[PHObject allocWithZone:zone] init];
	NSMutableArray * prop = [[NSMutableArray alloc] init];
	for (PHObjectProperty * property in properties)
	{
		PHObjectProperty * newProp = [[property copy] autorelease];
		if (property==classProperty)
			obj.classProperty = newProp;
		if (property==posXProperty)
			obj.posXProperty = newProp;
		if (property==posYProperty)
			obj.posYProperty = newProp;
		if (property==rotationProperty)
			obj.rotationProperty = newProp;
		[prop addObject:newProp];
	}
	return obj;
}

@end
