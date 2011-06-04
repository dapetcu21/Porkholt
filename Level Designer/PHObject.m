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
#import "ObjectController.h"
#import "BrowserTreeNode.h"

@implementation PHObject

@synthesize classProperty;
@synthesize posXProperty;
@synthesize posYProperty;
@synthesize posProperty;
@synthesize readOnly;
@synthesize rotationProperty;
@synthesize view;
@synthesize controller;
@synthesize imagesProperty;

-(id)init
{
	if (self = [super init])
	{
		selected = NO;
		properties = [[NSMutableArray alloc] init];
		[properties addObject:classProperty=[PHObjectProperty mandatoryPropertyWithValue:@"PHObject" ofType:kPHObjectPropertyString forKey:@"class"]];
        posXProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"x"];
        posYProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"y"];
        [properties addObject:posProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSArray arrayWithObjects:posXProperty,posYProperty,nil] ofType:kPHObjectPropertyTree forKey:@"pos"]];
		[properties addObject:rotationProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"rotation"]];
        [properties addObject:imagesProperty=[PHObjectProperty mandatoryPropertyWithValue:nil ofType:kPHObjectPropertyArray forKey:@"images"]];
        
	}
	return self;
}

-(void)dealloc
{
    [properties release];
	[super dealloc];
}

-(PHObjectProperty*)propertyForKey:(NSString*)key
{
    for (PHObjectProperty * prop in properties)
        if ([prop.key isEqualToString:key])
            return prop;
    return nil;
}

-(void)searchForVitalProperties
{
    posProperty = [self propertyForKey:@"pos"];
    classProperty = [self propertyForKey:@"class"];
    rotationProperty = [self propertyForKey:@"rotation"];
    posXProperty = [posProperty propertyForKey:@"x"];
    posYProperty = [posProperty propertyForKey:@"y"];
    imagesProperty = [self propertyForKey:@"images"];
    posProperty.mandatory = YES;
    posXProperty.mandatory = YES;
    posYProperty.mandatory = YES;
    rotationProperty.mandatory = YES;
    classProperty.mandatory = YES;
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
				BOOL tree = NO;
				int type;
				
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
				} else
                if (lua_istable(L, -1))
                {
                    tree=YES;
                    value = nil;
                    type = kPHObjectPropertyTree;
                }
                
                if ([key isEqualToString:@"physics"])//||
                    //[key isEqualToString:@"images"])
                    key = nil;
				
				if (key&&(value||tree))
				{
                    PHObjectProperty * prop = [PHObjectProperty propertyWithValue:value ofType:type forKey:key];
                    prop.parentObject = self;
					
                    [properties addObject:prop];
                    
                    if (tree)
                        [prop loadFromLua:L];
				}
                lua_pop(L,1);
			}
			lua_pop(L, 1);
		}
        [properties sortUsingSelector:@selector(compare:)];
        [self searchForVitalProperties];
	}
	return self;
}

-(void)saveNode:(PHObjectProperty*)prop withPath:(NSString*)path toFile:(NSMutableString*)file
{
    [file appendFormat:@"%@ = ",path];
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
        case kPHObjectPropertyTree:
        {
            [file appendString:@"{};\n"];
            NSArray * arr = prop.childNodes;
            for ( PHObjectProperty * nde in arr) 
            {
                [self saveNode:nde withPath:[path stringByAppendingFormat:@".%@",nde.key] toFile:file];
            }
            break;
        }
        case kPHObjectPropertyArray:
        {
            NSArray * arr = prop.childNodes;
            int n = [arr count];
            [file appendFormat:@"{n=%d;};\n",n];
            for (int i =0; i<n; i++)
            {
                PHObjectProperty * nde = [arr objectAtIndex:i];
                [self saveNode:nde withPath:[path stringByAppendingFormat:@"[%d]",i] toFile:file];
            }
            break;
        }
    }
}
                         
-(void)saveToFile:(NSMutableString*)file
{
	[file appendFormat:@"\nobj = objectWithClass(\"%@\");\n",self.className];
    
	for ( PHObjectProperty * prop in properties) 
	{
		if ([prop.key isEqual:@"class"]) continue;
		[self saveNode:prop withPath:[@"obj" stringByAppendingFormat:@".%@",prop.key] toFile:file];
	}
	[file appendFormat:@"obj.levelDes = true;\naddObject(obj);\n"];
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init])
	{		
		[self setProperties:[[aDecoder decodeObjectForKey:@"properties"] retain]];
		[self searchForVitalProperties];
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
    for (PHObjectProperty * prop in properties)
        prop.parentObject = nil;
	[properties release];
	properties = [[NSMutableArray alloc] initWithArray:obj];
    for (PHObjectProperty * prop in properties)
        prop.parentObject = self;
}

-(NSMutableArray*)properties
{
	return properties;
}

-(void)setClassName:(NSString *)cn
{
	[classProperty setValue:cn];
	[self modified];
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

-(void)undoableSetPosition:(NSPoint)pos
{
    [[[self.controller undoManager] prepareWithInvocationTarget:self] undoableSetPosition:[self position]];
    [self setPosition:pos];
}

-(void)setPosition:(NSPoint)pos
{
	posXProperty.doubleValue = pos.x;
	posYProperty.doubleValue = pos.y;
	[self positionChanged];
	[self modified];
}

-(void)undoableMove:(NSPoint)mv
{
    [[[self.controller undoManager] prepareWithInvocationTarget:self] undoableSetPosition:[self position]];
    [self move:mv];
}

-(void)move:(NSPoint)mv
{
	posXProperty.doubleValue = posXProperty.doubleValue+mv.x;
	posYProperty.doubleValue = posYProperty.doubleValue+mv.y;
	[self positionChanged];
	[self modified];
}

-(void)modified
{
	[controller objectChanged:self];
}

-(double)rotation
{
	return rotationProperty.doubleValue;
}

-(void)setRotation:(double)rot
{
	rotationProperty.doubleValue = rot;
	[self positionChanged];
	[self modified];
}

+(Class)viewClassForLuaClass:(NSString*)class
{
	return [ObjectView class];
}

-(void)rebuildView
{
	[self retain];
	Class cls = [[self class] viewClassForLuaClass:classProperty.stringValue];
	if (!view || ![[view class] isEqual:cls])
	{
		if (view)
		{
			view.object = nil;
		}
		view = [[cls alloc] init];
		view.object = self;
	}
	[view setFrame:NSMakeRect(-0.04, -0.04, 0.08f, 0.08f)];
	[self positionChanged];
	[self release];
}

-(void)positionChanged
{
	if (!view) return;
	NSSize size = view.frame.size;
	[view setFrameCenterRotation:0];
	[view setFrameOrigin:NSMakePoint(posXProperty.doubleValue-size.width/2, posYProperty.doubleValue-size.height/2)];
	[view setFrameCenterRotation:rotationProperty.doubleValue];
}

-(BOOL)editable
{
	return !readOnly;
}

-(void)setEditable:(BOOL)sa
{
	readOnly=!sa;
}

-(BOOL)isSelected
{
	return selected;
}

-(void)updateSelected:(BOOL)val
{
	selected = val;
}

-(void)setSelected:(BOOL)val
{
	if (val==selected)
		return;
	if (val)
		[[controller arrayController] addSelectedObjects:[NSArray arrayWithObject:self]];
	else 
		[[controller arrayController] removeSelectedObjects:[NSArray arrayWithObject:self]];
}

-(id)copyWithZone:(NSZone *)zone
{
	PHObject * obj = [[PHObject allocWithZone:zone] init];
	NSMutableArray * prop = [[[NSMutableArray alloc] init] autorelease];
	for (PHObjectProperty * property in properties)
	{
		PHObjectProperty * newProp = [[property copy] autorelease];
		[prop addObject:newProp];
	}
    [obj setProperties:prop];
	obj.controller = controller;
    [obj searchForVitalProperties];
	return obj;
}

@end
