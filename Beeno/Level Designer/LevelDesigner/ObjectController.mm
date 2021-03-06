/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "ObjectController.h"
#import "InterfaceController.h"
#import "PLEntity.h"
#import "PLObject.h"
#import "PLJoint.h"
#import "WorldController.h"
#import "StatusController.h"
#import "PLDocument.h"
#import "PLBezier.h"
#import "SubentityController.h"
#import "PLImage.h"
#import "PLFixture.h"
#import "PLBezier.h"
#import "PLProperty.h"

@implementation ObjectController

- (id)init
{
    self = [super initWithArrays:ObjectController_numberOfArrays andPasteboardType:PLObjectPBoardType];
    if (self)
        showMarkers = showFixtures = showJoints = showImages = YES;
    return self;
}

-(NSArray*)objects
{
    return arrays[0];
}

-(NSArray*)joints
{
    return arrays[1];
}

-(Class)classForArray:(NSUInteger)array
{
    switch (array) {
        case 0:
            return [PLObject class];
        case 1:
            return [PLJoint class];
        case 2:
            return [PLBezier class];
        default:
            return NULL;
    }
}

-(void)entityChanged:(PLEntity *)e
{
    [super entityChanged:e];
    if ([e isKindOfClass:[PLJoint class]])
        [worldController jointsChanged];
}

-(void)arrayChanged:(NSUInteger)array
{
    [super arrayChanged:array];
    if (array==0)
        [worldController objectsChanged];
    if (array==1)
        [worldController jointsChanged];
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [super selectionForArrayChanged:array];
    if (array==0)
        [worldController objectSelectionChanged];
    if (array==1)
        [worldController jointSelectionChanged];
    if (![self isObjectModePossible])
        objectMode = NO;
    [statusController flagsChanged];
    [worldController flagsChanged];
}

#pragma Loading and saving to file
-(void)lua:(lua_State*)L setPath:(NSString*)path
{
	lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
    NSString * cur_path = [NSString stringWithUTF8String:lua_tostring( L, -1 )]; 
    cur_path = [cur_path stringByAppendingString:@";"];
    cur_path = [cur_path stringByAppendingString:path];
    lua_pop( L, 1 ); 
    lua_pushstring( L, [cur_path UTF8String]);
    lua_setfield( L, -2, "path" ); 
    lua_pop( L, 1 );
}

-(void)loadFromFile:(NSURL *)url
{
	NSMutableArray * robjs = [[[NSMutableArray alloc] init] autorelease];
	NSMutableArray * objs = [[[NSMutableArray alloc] init] autorelease];
	
	int error;
	lua_State * L = lua_open();   /* opens Lua */
	luaL_openlibs(L);
	
	[self lua:L setPath:[NSString stringWithFormat:@"%@/?.lua;%@/?.lua",[[NSBundle mainBundle] resourcePath],[url path]]];
	
	if (url)
	{
		error = luaL_loadfile(L, [[[url URLByAppendingPathComponent:@"init.lua"] path] UTF8String]) || lua_pcall(L, 0, 0, 0);
		if (error) {
			NSLog(@"Lua: %s",lua_tostring(L,-1));
			lua_pop(L, 1);
		}
	}
	error = luaL_loadfile(L, [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"level_des.lua"] UTF8String]) || lua_pcall(L, 0, 0, 0);
	if (error) {
		NSLog(@"Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
	} 
	
	lua_getglobal(L, "objects");
	if (lua_istable(L, -1))
	{
		int n = 0;
        lua_getfield(L, -1, "n");
		if (lua_isnumber(L, -1))
			n = lua_tonumber(L, -1);
		lua_pop(L,1);
		
		for (int i=0; i<n; i++)
		{
			lua_getglobal(L, "describeObject");
			lua_pushnumber(L, i);
			lua_gettable(L, -3);
			if (lua_istable(L, -1))
			{
				error = lua_pcall(L, 1, 1, 0);
				if (error)
				{
					NSLog(@"Lua: %s",lua_tostring(L,-1));
				} else {
					if (lua_istable(L,-1))
					{
						PLObject * obj = [[[PLObject alloc] initFromLua:L] autorelease];
						obj.owner = self;
						if (obj.readOnly)
							[robjs addObject:obj];
						else
							[objs addObject:obj];
					}
				}
				lua_pop(L,1);
			} else {
				lua_pop(L,1);
				lua_pop(L,1);
			}
		}
	}
	lua_pop(L,1);
    
    NSArray * totalobjs = [robjs arrayByAddingObjectsFromArray:objs];
    
    NSMutableArray * rjoints = [NSMutableArray array];
    NSMutableArray * joints = [NSMutableArray array];
    
    lua_getglobal(L, "joints");
    if (lua_istable(L, -1))
    {
        int n = 0;
        lua_getfield(L, -1, "n");
		if (lua_isnumber(L, -1))
			n = lua_tonumber(L, -1);
		lua_pop(L,1);
        
        for (int i=0; i<n; i++)
        {
            lua_pushnumber(L, i);
            lua_gettable(L, -2);
            if (lua_istable(L, -1))
            {
                PLJoint * joint = [[[PLJoint alloc] initFromLua:L] autorelease];
                if (joint.body1index != -1)
                    joint.body1 = [totalobjs objectAtIndex:joint.body1index];
                if (joint.body2index != -1)
                    joint.body2 = [totalobjs objectAtIndex:joint.body2index];
                joint.owner = self;
                if (joint.readOnly)
                    [rjoints addObject:joint];
                else
                    [joints addObject:joint];
            }
            lua_pop(L,1);
        }
    }
    lua_pop(L,1);
    NSArray * totaljoints = [rjoints arrayByAddingObjectsFromArray:joints];
    
    PHMessage::messageWithName("luaDestroy")->broadcast(nil, L);
	lua_close(L);
    
    [self beginCommitGrouping];
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [arrays[0] count])] fromArray:0];
	[self insertEntities:totalobjs atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [totalobjs count])] inArray:0];
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [arrays[1] count])] fromArray:1];
	[self insertEntities:totaljoints atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [totaljoints count])] inArray:1];
    [self endCommitGrouping];
    [undoManager removeAllActions];
}

-(void)saveToFile:(NSMutableString*)file
{
	[file appendString:@"--This file was generated with the Porkholt Level Designer\n"];
	[file appendString:@"--Do not modify this file. If you do, please note that this\n"];
	[file appendString:@"--file is reset every time you use the Level Designer\n\n"];
    
    NSMutableArray * beziers = [[[NSMutableArray alloc] init] autorelease];

#define indexForBezier(x) \
    PLBezier * b = (x); \
    NSUInteger index; \
    NSUInteger n = [beziers count]; \
    for (index = 0; index<n; index++) \
        if ([b isEqual:[beziers objectAtIndex:index]]) \
            break; \
    if (index==n) \
    { \
        [file appendFormat:@"local bezierCurve%u = ",(unsigned int)index]; \
        [b writeToFile:file]; \
        [beziers addObject:b]; \
        [file appendString:@"\n"]; \
    }
    
    int onr = 0;
    
	for (PLObject * object in arrays[0])
		if (!object.readOnly)
        {   
            NSArray * fixtures = [[object subentityModel] fixtures];
            for (PLFixture * fx in fixtures)
            {
                if (fx.shape == PLFixtureFreestyle && fx.bezierCurve)
                {
                    indexForBezier(fx.bezierCurve);
                    fx.bezierCurveIndex = index;
                }
            }
            NSArray * images = [[object subentityModel] images];
            for (PLImage * img in images)
            {
                if (img.bezierCurve)
                {
                    indexForBezier(img.bezierCurve);
                    img.bezierCurveIndex = index;
                }
            }
            PLProperty * p = [object.rootProperty propertyWithKey:@"scripting"];
            if (!p || !p.type==PLPropertyString)
            {
                [object setObjectName:[NSString stringWithFormat:@"obj%d",onr]];
                onr++;
            } else 
                [object setObjectName:[p stringValue]];
			[object writeToFile:file];
        }
    BOOL declared = NO;
    for (PLJoint * joint in arrays[1])
        if (!joint.readOnly)
        {
            if (!declared)
            {
                declared = YES;
                [file appendString:@"local joint\n"];
            }
            [joint writeToFile:file];
        }
}

-(BOOL)showMarkers
{
    return showMarkers;
}

-(BOOL)showFixtures
{
    return showFixtures;
}

-(BOOL)showImages
{
    return showImages;
}

-(BOOL)showJoints
{
    return showJoints;
}

-(BOOL)objectMode
{
    return objectMode;
}

-(void)setShowMarkers:(BOOL)sm
{
    if (showMarkers==sm) return;
    showMarkers = sm;
    [statusController flagsChanged];
    [worldController flagsChanged];
}

-(void)setShowImages:(BOOL)sm
{
    if (showImages==sm) return;
    showImages = sm;
    [statusController flagsChanged];
    [worldController flagsChanged];
}

-(void)setShowFixtures:(BOOL)sm
{
    if (showFixtures==sm) return;
    showFixtures = sm;
    [statusController flagsChanged];
    [worldController flagsChanged];
}

-(void)setShowJoints:(BOOL)sm
{
    if (showJoints==sm) return;
    showJoints = sm;
    [statusController flagsChanged];
    [worldController flagsChanged];
}

-(void)setObjectMode:(BOOL)om
{
    if (objectMode==om) return;
    objectMode = om;
    [statusController flagsChanged];
    [worldController flagsChanged];
}

-(BOOL)isObjectModePossible
{
    PLEntity * e = [self selectedEntity];
    return [e isKindOfClass:[PLObject class]] && !e.readOnly;
}

-(NSURL*)fileURL
{
    return [document fileURL];
}

@end
