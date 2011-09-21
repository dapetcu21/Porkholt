//
//  ObjectController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "ObjectController.h"
#import "InterfaceController.h"
#import "PLEntity.h"
#import "PLObject.h"
#import "PLJoint.h"

@implementation ObjectController

- (id)init
{
    if (self = [super initWithArrays:ObjectController_numberOfArrays andPasteboardType:PLObjectPBoardType])
    {
        
    }
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
        default:
            return NULL;
    }
}

-(void)arrayChanged:(NSUInteger)array
{
    [interfaceController arrayChanged:array];
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [interfaceController selectionForArrayChanged:array];
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
		lua_pushstring(L, "n");
		lua_gettable(L, -2);
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
	
	lua_close(L);
	
    PLObject * obj = [[[PLObject alloc] initFromLua:L] autorelease];
    obj.owner = self;
    obj.readOnly = YES;
    [robjs addObject:obj];
    
	NSUInteger n = [robjs count];
    NSUInteger m = [objs count];
    
    [self beginCommitGrouping];
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [arrays[0] count])] fromArray:0];
	[self insertEntities:[robjs arrayByAddingObjectsFromArray:objs] atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, n+m)] inArray:0];
    [self endCommitGrouping];
    [undoManager removeAllActions];
}

-(void)saveToFile:(NSMutableString*)file
{
	[file appendString:@"--This file was generated with the Porkholt Level Designer\n"];
	[file appendString:@"--Do not modify this file. If you do, please note that this\n"];
	[file appendString:@"--file is reset every time you use the Level Designer\n"];
	for (PLObject * object in arrays[0])
		if (!object.readOnly)
			[object saveToFile:file];
	[file appendString:@"\n"];
}

@end
