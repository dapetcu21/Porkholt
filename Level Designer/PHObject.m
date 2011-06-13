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
#import "InheritanceController.h"

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
@synthesize fixturesProperty;
@synthesize physicsProperty;
@synthesize initialSelected;

-(id)init
{
	if (self = [super init])
	{
		selected = NO;
		properties = [[NSMutableArray alloc] init];
		[properties addObject:classProperty=[PHObjectProperty mandatoryPropertyWithValue:@"PHLObject" ofType:kPHObjectPropertyString forKey:@"class"]];
        posXProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"x"];
        posYProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"y"];
        [properties addObject:posProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSArray arrayWithObjects:posXProperty,posYProperty,nil] ofType:kPHObjectPropertyTree forKey:@"pos"]];
		[properties addObject:rotationProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSNumber numberWithInt:0] ofType:kPHObjectPropertyNumber forKey:@"rotation"]];
        [properties addObject:imagesProperty=[PHObjectProperty mandatoryPropertyWithValue:nil ofType:kPHObjectPropertyArray forKey:@"images"]];
        fixturesProperty = [PHObjectProperty mandatoryPropertyWithValue:nil ofType:kPHObjectPropertyArray forKey:@"fixtures"];
        [properties addObject:physicsProperty=[PHObjectProperty mandatoryPropertyWithValue:[NSArray arrayWithObject:fixturesProperty] ofType:kPHObjectPropertyTree forKey:@"physics"]];
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
    physicsProperty = [self propertyForKey:@"physics"];
    fixturesProperty = [physicsProperty propertyForKey:@"fixtures"];
    posProperty.mandatory = YES;
    posXProperty.mandatory = YES;
    posYProperty.mandatory = YES;
    rotationProperty.mandatory = YES;
    classProperty.mandatory = YES;
    imagesProperty.mandatory = YES;
    physicsProperty.mandatory = YES;
    fixturesProperty.mandatory = YES;
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
                
                //if ([key isEqualToString:@"physics"])//||
                    //[key isEqualToString:@"images"])
                //    key = nil;
				
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

-(BOOL)saveNode:(PHObjectProperty*)prop withPath:(NSString*)path toFile:(NSMutableString*)file andModel:(PHObjectProperty*)model
{
    if (model && prop.type != kPHObjectPropertyArray && prop.type != kPHObjectPropertyTree && [prop.value isEqual:model.value]) return NO;
    NSUInteger start = [file length];
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
            PHObjectProperty * posX, * posY, * posW, * posH;
            posX = [prop propertyForKey:@"x"];
            posY = [prop propertyForKey:@"y"];
            posW = [prop propertyForKey:@"width"];
            posH = [prop propertyForKey:@"height"];
            if (posX.type != kPHObjectPropertyNumber) posX = nil;
            if (posY.type != kPHObjectPropertyNumber) posY = nil;
            if (posW.type != kPHObjectPropertyNumber) posW = nil;
            if (posH.type != kPHObjectPropertyNumber) posH = nil;
            
            PHObjectProperty * mposX, * mposY, * mposW, * mposH;
            mposX = [model propertyForKey:@"x"];
            mposY = [model propertyForKey:@"y"];
            mposW = [model propertyForKey:@"width"];
            mposH = [model propertyForKey:@"height"];
            if (mposX.type != kPHObjectPropertyNumber) mposX = nil;
            if (mposY.type != kPHObjectPropertyNumber) mposY = nil;
            if (mposW.type != kPHObjectPropertyNumber) mposW = nil;
            if (mposH.type != kPHObjectPropertyNumber) mposH = nil;
            
            NSArray * arr = prop.childNodes;
            if ([arr count]==2 && posX && posY)
            {
                if (mposX && mposY && mposX.doubleValue==posX.doubleValue && mposY.doubleValue==posY.doubleValue)
                    [file deleteCharactersInRange:NSMakeRange(start,[file length]-start)];
                else
                    [file appendFormat:@"point(%lf,%lf);\n",posX.doubleValue,posY.doubleValue];
            }
            else 
                if ([arr count]==4 && posX && posY && posW && posH)
                {
                    if (mposX && mposY && mposW && mposH && mposX.doubleValue==posX.doubleValue && mposY.doubleValue==posY.doubleValue && mposW.doubleValue==posW.doubleValue && mposH.doubleValue==posH.doubleValue)
                        [file deleteCharactersInRange:NSMakeRange(start,[file length]-start)];
                    else
                        [file appendFormat:@"rect(%lf,%lf,%lf,%lf);\n",posX.doubleValue,posY.doubleValue,posW.doubleValue,posH.doubleValue];
                }
            else
            {
                [file appendString:@"{};\n"];
                BOOL doit = NO;
                for ( PHObjectProperty * nde in arr) 
                {
                    doit = doit || [self saveNode:nde withPath:[path stringByAppendingFormat:@".%@",nde.key] toFile:file andModel:[model propertyForKey:nde.key]];
                }
                if (!doit)
                    [file deleteCharactersInRange:NSMakeRange(start,[file length]-start)];
            }
            break;
        }
        case kPHObjectPropertyArray:
        {
            NSArray * arr = prop.childNodes;
            int n = [arr count];
            [file appendFormat:@"{n=%d;};\n",n];
            BOOL doit = NO;
            NSArray * arr2 = model.childNodes;
            for (int i =0; i<n; i++)
            {
                PHObjectProperty * nde = [arr objectAtIndex:i];
                doit = doit || [self saveNode:nde withPath:[path stringByAppendingFormat:@"[%d]",i] toFile:file andModel:((i<[arr2 count])?[arr2 objectAtIndex:i]:nil)];
            }
            if (!doit)
                [file deleteCharactersInRange:NSMakeRange(start,[file length]-start)];
            break;
        }
    }
    return YES;
}

-(void)saveFixture:(PHObjectProperty*)img toFile:(NSMutableString*)file
{
    if (img.type != kPHObjectPropertyTree) return;
    NSArray * arr = [img value];
    PHObjectProperty * shape = [img propertyForKey:@"shape"];
    if (!shape || shape.type!=kPHObjectPropertyString) return;
    
    if ([shape.value isEqualToString:@"circle"])
    {
        double rad=0;
        
        PHObjectProperty * crad = [img propertyForKey:@"circleR"];
        if (crad && crad.type==kPHObjectPropertyNumber)
            rad = crad.doubleValue;
        
        int nr = 0;
        for (PHObjectProperty * prop in arr)
        {
            if ([prop.key isEqualToString:@"circleR"]||[prop.key isEqualToString:@"shape"]) continue;
            nr++;
        }
        
        if (nr)
        {
            [file appendFormat:@"op = {};\n"];
            for (PHObjectProperty * prop in arr)
            {
                if ([prop.key isEqualToString:@"circleR"]||[prop.key isEqualToString:@"shape"]) continue;
                [self saveNode:prop withPath:[@"op" stringByAppendingFormat:@".%@",prop.key] toFile:file andModel:nil];
            }
        }    
        [file appendFormat:@"objectAddCircle(obj,%lf%@);\n",rad,nr?@",op":@""];
    }
    if ([shape.value isEqualToString:@"box"])
    {
        double x=0,y=0,w=0,h=0;
        
        PHObjectProperty * pos = [img propertyForKey:@"box"];
        if (!pos || pos.type!=kPHObjectPropertyTree) return;
        
        PHObjectProperty * posX = [pos propertyForKey:@"x"];
        if (posX && posX.type==kPHObjectPropertyNumber)
            x = posX.doubleValue;
        PHObjectProperty * posY = [pos propertyForKey:@"y"];
        if (posY && posY.type==kPHObjectPropertyNumber)
            y = posY.doubleValue;
        PHObjectProperty * posW = [pos propertyForKey:@"width"];
        if (posW && posW.type==kPHObjectPropertyNumber)
            w = posW.doubleValue;
        PHObjectProperty * posH = [pos propertyForKey:@"height"];
        if (posH && posH.type==kPHObjectPropertyNumber)
            h = posH.doubleValue;
        
        int nr = 0;
        for (PHObjectProperty * prop in arr)
        {
            if ([prop.key isEqualToString:@"box"]||[prop.key isEqualToString:@"shape"]) continue;
            nr++;
        }
        
        if (nr)
        {
            [file appendFormat:@"op = {};\n"];
            for (PHObjectProperty * prop in arr)
            {
                if ([prop.key isEqualToString:@"box"]||[prop.key isEqualToString:@"shape"]) continue;
                [self saveNode:prop withPath:[@"op" stringByAppendingFormat:@".%@",prop.key] toFile:file andModel:nil];
            }
        }    
        [file appendFormat:@"objectAddBox(obj,%lf,%lf,%lf,%lf%@);\n",x,y,w,h,nr?@",op":@""];
    }
}

-(void)saveImage:(PHObjectProperty*)img toFile:(NSMutableString*)file
{
    if (img.type != kPHObjectPropertyTree) return;
    NSArray * arr = [img value];
    PHObjectProperty * pos = [img propertyForKey:@"pos"];
    if (!pos || pos.type!=kPHObjectPropertyTree) return;
    double x=0,y=0,w=0,h=0;
    NSString * fn = @"";
    
    PHObjectProperty * posX = [pos propertyForKey:@"x"];
    if (posX && posX.type==kPHObjectPropertyNumber)
        x = posX.doubleValue;
    PHObjectProperty * posY = [pos propertyForKey:@"y"];
    if (posY && posY.type==kPHObjectPropertyNumber)
        y = posY.doubleValue;
    PHObjectProperty * posW = [pos propertyForKey:@"width"];
    if (posW && posW.type==kPHObjectPropertyNumber)
        w = posW.doubleValue;
    PHObjectProperty * posH = [pos propertyForKey:@"height"];
    if (posH && posH.type==kPHObjectPropertyNumber)
        h = posH.doubleValue;
    PHObjectProperty * fnprop = [img propertyForKey:@"filename"];
    if (fnprop && fnprop.type==kPHObjectPropertyString)
        fn = fnprop.stringValue;
    
    int nr = 0;
    for (PHObjectProperty * prop in arr)
    {
        if ([prop.key isEqualToString:@"pos"]||[prop.key isEqualToString:@"filename"]) continue;
        nr++;
    }
    
    if (nr)
    {
        [file appendFormat:@"op = {};\n"];
        for (PHObjectProperty * prop in arr)
        {
            if ([prop.key isEqualToString:@"pos"]||[prop.key isEqualToString:@"filename"]) continue;
            [self saveNode:prop withPath:[@"op" stringByAppendingFormat:@".%@",prop.key] toFile:file andModel:nil];
        }
    }    
    [file appendFormat:@"objectAddImage(obj,[[%@]],%lf,%lf,%lf,%lf%@);\n",fn,x,y,w,h,nr?@",op":@""];
}
                         
-(void)saveToFile:(NSMutableString*)file
{
	[file appendFormat:@"\nobj = objectWithClass(\"%@\");\n",self.className];
    [file appendFormat:@"obj.pos = point(%lf,%lf);\n",self.posXProperty.doubleValue,self.posYProperty.doubleValue];
    
    PHObject * model = [[InheritanceController sharedInstance] modelForClass:self.className];
    
	for ( PHObjectProperty * prop in properties) 
	{
		if ([prop.key isEqual:@"class"]) continue;
        if ([prop.key isEqual:@"pos"]) continue;
        if ([prop.key isEqual:@"images"]) continue;
        if ([prop.key isEqual:@"physics"]) continue;
		[self saveNode:prop withPath:[@"obj" stringByAppendingFormat:@".%@",prop.key] toFile:file andModel:[model propertyForKey:prop.key]];
	}
    
    for ( PHObjectProperty  * prop in physicsProperty.childNodes)
    {
        if ([prop.key isEqual:@"fixtures"]) continue;
        [self saveNode:prop withPath:[@"obj.physics." stringByAppendingString:prop.key] toFile:file andModel:[model.physicsProperty propertyForKey:prop.key]];
    }
    
    NSArray * arr = model.imagesProperty.childNodes;
    int i = 0;
    NSArray * images = [self.imagesProperty value];
    for (PHObjectProperty * prop in images)
    {
        if (i>=[arr count])
            [self saveImage:prop toFile:file];
        i++;
    }
    
    arr = model.fixturesProperty.childNodes;
    i = 0;
    NSArray * fixtures = [self.fixturesProperty value];
    for (PHObjectProperty * prop in fixtures)
    {
        if (i>=[arr count])
            [self saveFixture:prop toFile:file];
        i++;
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

-(void)undoableSetRotation:(double)rot
{
    [[[self.controller undoManager] prepareWithInvocationTarget:self] undoableSetRotation:self.rotation];
    [self setRotation:rot];
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
    [view modified];
}

-(void)hardModified
{
    [view hardModified];
}

-(double)rotation
{
	return rotationProperty.doubleValue;
}

-(void)setRotation:(double)rot
{
    while (rot<0)
        rot+=360;
    while (rot>=360)
        rot-=360;
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
    [view modified];
	[self positionChanged];
	[self release];
}

-(void)positionChanged
{
	[view updatePosition];
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
    [view setSelected:val];
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
