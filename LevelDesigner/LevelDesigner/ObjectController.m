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
@synthesize undoManager;

- (id)init
{
    self = [super init];
    if (self) {
        NSUInteger i;
        for (i=0; i<ObjectController_numberOfArrays; i++)
        {
            arrays[i] = [[NSMutableArray alloc] init];
            selection[i] = [[NSMutableIndexSet alloc] init];
            needsReindexing[i] = NO;
            readOnly[i] = 0;
        }
    }
    
    return self;
}

-(void)dealloc
{
    NSUInteger i;
    for (i=0; i<ObjectController_numberOfArrays; i++)
    {
        [arrays[i] release];
        [selection[i] release];
    }
    [undoManager release];
    [super dealloc];
}

#pragma mark Array access
-(NSArray*)objects
{
    return arrays[0];
}

-(NSArray*)joints
{
    return arrays[1];
}

-(NSArray*)arrayAtIndex:(NSUInteger)array
{
    return arrays[array];
}

-(NSUInteger)readOnlyEntitiesInArray:(NSUInteger)array
{
    return readOnly[array];
}

-(NSArray*)entitiesForIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array
{
    NSMutableArray * a = [NSMutableArray arrayWithCapacity:[indexes count]];
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        [a addObject:[arrays[array] objectAtIndex:idx]];
    }];
    return a;
}

-(PLEntity*)entityForIndex:(NSUInteger)index inArray:(NSUInteger)array
{
    return (PLEntity*)[arrays[array] objectAtIndex:index];
}

-(void)reindexArray:(NSUInteger)array;
{
    if (needsReindexing[array])
    {
        NSArray * a = arrays[array];
        NSUInteger i;
        NSUInteger n = [a count];
        for(i=0; i<n; i++)
            [(PLEntity*)[a objectAtIndex:i] setIndex:i andArray:array];
    }
}

-(NSUInteger)indexForEntity:(PLEntity*)entity inArray:(NSUInteger)array
{
    [self reindexArray:array];
    if (entity.array != array || entity.index == NSNotFound)
        return NSNotFound;
    return entity.index;
}

-(NSIndexSet*)indexesForEntities:(NSArray*)b inArray:(NSUInteger)array
{
    NSMutableIndexSet * is = [NSMutableIndexSet indexSet];
    [self reindexArray:array];
    for (PLEntity * e in b)
    {
        if (e.array != array || e.index == NSNotFound)
            continue;
        [is addIndex:e.index];
    }
    return is;
}

#pragma mark Array editing

-(void)arrayChanged:(NSUInteger)array
{
    [interfaceController arrayChanged:array];
}

-(void)insertEntity:(PLEntity*)entity atIndex:(NSUInteger)index inArray:(NSUInteger)array
{
    [self insertEntities:[NSArray arrayWithObject:entity] atIndexes:[NSIndexSet indexSetWithIndex:index] inArray:array];
}

-(void)insertEntities:(NSArray*)entities atIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array
{
    if ([indexes count]==0) return;
    [[undoManager prepareWithInvocationTarget:self] removeEntitiesAtIndexes:indexes fromArray:array];
    for (PLEntity * e in entities)
        if (e.readOnly)
            readOnly[array]++;
    [arrays[array] insertObjects:entities atIndexes:indexes];
    needsReindexing[array] = YES;
    [self arrayChanged:array];
}

-(void)removeEntityAtIndex:(NSUInteger)index fromArray:(NSUInteger)array
{
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndex:index] fromArray:array];
}

-(void)removeEntitiesAtIndexes:(NSIndexSet*)indexes fromArray:(NSUInteger)array
{
    if ([indexes count]==0) return;
    [[undoManager prepareWithInvocationTarget:self] insertEntities:[self entitiesForIndexes:indexes inArray:array] atIndexes:indexes inArray:array];
    NSMutableArray * a = arrays[array];
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[a objectAtIndex:idx];
        [e resetIndex];
        if (e.readOnly)
            readOnly[array]--;
    }];
    [a removeObjectsAtIndexes:indexes];
    needsReindexing[array] = YES;
    [self arrayChanged:array];
}

-(void)removeEntity:(PLEntity*)entity fromArray:(NSUInteger)array
{
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndex:[self indexForEntity:entity inArray:array]] fromArray:array];
}

-(void)removeEntities:(NSArray*)entities fromArray:(NSUInteger)array
{
    [self removeEntitiesAtIndexes:[self indexesForEntities:entities inArray:array] fromArray:array];
}

#pragma mark Array selection

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [interfaceController selectionForArrayChanged:array];
}

-(NSIndexSet*)selectionForArray:(NSUInteger)array
{
    return selection[array];
}

-(void)clearSelectionForArray:(NSUInteger)array
{
    [selection[array] removeAllIndexes];
    [self selectionForArrayChanged:array];
}

-(void)clearSelection
{
    NSUInteger i;
    for (i=0; i<ObjectController_numberOfArrays; i++)
        [self clearSelectionForArray:i];
}

-(void)setSelectedIndex:(NSUInteger)index forArray:(NSUInteger)array
{
    [selection[array] removeAllIndexes];
    [selection[array] addIndex:index];
    [self selectionForArrayChanged:array];
}

-(void)setSelectedEntity:(PLEntity*)entity forArray:(NSUInteger)array
{
    [self setSelectedIndex:[self indexForEntity:entity inArray:array] forArray:array];
}

-(void)setSelectedIndexes:(NSIndexSet*)indexes forArray:(NSUInteger)array
{
    [selection[array] removeAllIndexes];
    [selection[array] addIndexes:indexes];
    [self selectionForArrayChanged:array];
}

-(void)setSelectedEntities:(NSArray*)entities forArray:(NSUInteger)array
{
    [self setSelectedIndexes:[self indexesForEntities:entities inArray:array] forArray:array];
}

-(void)insertIndex:(NSUInteger)index inSelectionForArray:(NSUInteger)array
{
    [selection[array] addIndex:index];
    [self selectionForArrayChanged:array];
}

-(void)removeIndex:(NSUInteger)index inSelectionForArray:(NSUInteger)array
{
    [selection[array] removeIndex:index];
    [self selectionForArrayChanged:array];
}

-(void)insertIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array
{
    [selection[array] addIndexes:indexes];
    [self selectionForArrayChanged:array];
}

-(void)removeIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array
{
    [selection[array] removeIndexes:indexes];
    [self selectionForArrayChanged:array];
}

-(void)insertEntity:(PLEntity*)entity inSelectionForArray:(NSUInteger)array
{
    [self insertIndex:[self indexForEntity:entity inArray:array] inSelectionForArray:array];
}

-(void)removeEntity:(PLEntity*)entity inSelectionForArray:(NSUInteger)array
{
    [self removeIndex:[self indexForEntity:entity inArray:array] inSelectionForArray:array];
}

-(void)insertEntities:(NSArray*)entities inSelectionForArray:(NSUInteger)array
{
    [self insertIndexes:[self indexesForEntities:entities inArray:array] inSelectionForArray:array];
}

-(void)removeEntities:(NSArray*)entities inSelectionForArray:(NSUInteger)array
{
    [self removeIndexes:[self indexesForEntities:entities inArray:array] inSelectionForArray:array];
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
	
    //PLObject * obj = [[[PLObject alloc] initFromLua:L] autorelease];
    //obj.owner = self;
    //obj.readOnly = YES;
    //[robjs addObject:obj];
    
	NSUInteger n = [robjs count];
    NSUInteger m = [objs count];
    
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, [arrays[0] count])] fromArray:0];
	[self insertEntities:[robjs arrayByAddingObjectsFromArray:objs] atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(0, n+m)] inArray:0];
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
