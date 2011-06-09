//
//  InheritanceController.m
//  Level Designer
//
//  Created by Marius Petcu on 6/8/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "InheritanceController.h"
#import "PHObject.h"

@implementation InheritanceController

-(void)loadFromLua
{
    L = lua_open();  
	luaL_openlibs(L);
	
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
    NSString * cur_path = [NSString stringWithUTF8String:lua_tostring( L, -1 )]; 
    cur_path = [cur_path stringByAppendingFormat:@";%@/?.lua",[[NSBundle mainBundle] resourcePath]];
    lua_pop( L, 1 ); 
    lua_pushstring( L, [cur_path UTF8String]);
    lua_setfield( L, -2, "path" ); 
    lua_pop( L, 1 );

    int error = luaL_loadfile(L, [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"level_des.lua"] UTF8String]) || lua_pcall(L, 0, 0, 0);
	if (error) {
		NSLog(@"Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
	}
}

- (id)init
{
    self = [super init];
    if (self) {
        [self loadFromLua];
        models = [[NSMutableDictionary alloc] init];
    }
    
    return self;
}


- (PHObject*)createModelForClass:(NSString*)class
{
    lua_getglobal(L, "describeObject");
    lua_getglobal(L, "objectWithClass");
    lua_pushstring(L, [class UTF8String]);
    int error = lua_pcall(L,1,1,0);
    if (error)
    {
        NSLog(@"Lua: %s",lua_tostring(L,-1));
        lua_pop(L,2);
        return nil;
    }
    error = lua_pcall(L,1,1,0);
    if (error)
    {
        NSLog(@"Lua: %s",lua_tostring(L,-1));
        lua_pop(L,1);
        return nil;
    }
    PHObject * object = [[PHObject alloc] initFromLua:L];
    lua_pop(L,1);
    [models setObject:object forKey:class];
    return object;
}

- (PHObject*)modelForClass:(NSString*)class
{
    PHObject * obj = [models objectForKey:class];
    if (!obj)
        obj = [self createModelForClass:class];
    return obj;
}

- (void)dealloc
{
    [models release];
    lua_close(L);
    [super dealloc];
}

+ (InheritanceController*)sharedInstance
{
    static InheritanceController * cntr = nil;
    if (!cntr)
        cntr = [[InheritanceController alloc] init];
    return cntr;
}

@end
