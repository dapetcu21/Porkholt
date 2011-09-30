//
//  PrototypeController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PrototypeController.h"
#import "PLProperty.h"
#import "PLImage.h"
#import "PLFixture.h"
#import "PLPrototype.h"

@implementation PrototypeController


-(void)luaSetPath:(NSString*)path
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

-(void)initLuaContext
{
	int error;
    L = lua_open();   /* opens Lua */
	luaL_openlibs(L);
	
	[self luaSetPath:[NSString stringWithFormat:@"%@/?.lua;",[[NSBundle mainBundle] resourcePath]]];
	
    error = luaL_loadfile(L, [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"init_common.lua"] UTF8String]) || lua_pcall(L, 0, 0, 0);
    if (error) {
        NSLog(@"Lua: %s",lua_tostring(L,-1));
        lua_pop(L, 1);
    }

    error = luaL_loadfile(L, [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"level_des.lua"] UTF8String]) || lua_pcall(L, 0, 0, 0);
	if (error) {
		NSLog(@"Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
	}
}

- (id)init
{
    self = [super init];
    if (self) {
        props = [[NSMutableDictionary alloc] init];
        [self initLuaContext];
    }
    
    return self;
}

-(PLPrototype*)prototypeForClass:(NSString*)className
{
    PLPrototype * p = (PLPrototype*)[props objectForKey:className];
    if (!p)
    {
        p = [[[PLPrototype alloc] init] autorelease];
        PLProperty * prop = [self createPropertyForClass:className];
        PLProperty * phy = [prop propertyWithKey:@"physics"];
        PLProperty * fixtures = [[phy propertyWithKey:@"fixtures"] retain];
        [phy removeProperty:fixtures];
        if (![phy childrenCount])
            [prop removeProperty:phy];
        
        PLProperty * images = [[prop propertyWithKey:@"images"] retain];
        [prop removeProperty:images];
        p.rootProperty = prop;
        p.images = [PLImage imagesFromProperty:images];
        p.fixtures = [PLFixture fixturesFromProperty:fixtures];
        [images release];
        [fixtures release];
        
        for (PLImage * img in p.images)
            img.readOnly = YES;
        for (PLFixture * fx in p.fixtures)
            fx.readOnly = YES;
        
        [prop removeProperty:[prop propertyWithKey:@"class"]];
        [prop removeProperty:[prop propertyWithKey:@"pos"]];
        [prop removeProperty:[prop propertyWithKey:@"rotation"]];
        
        [props setObject:p forKey:className];
    }
    return p;
}

-(PLProperty*)createPropertyForClass:(NSString*)className
{
    int error;
    
    lua_getglobal(L, "describeObject");
    lua_getglobal(L, "objectWithClass");
    if (!lua_isfunction(L, -1) || !lua_isfunction(L, -2))
    {
        lua_pop(L,2);
        return nil;
    }
    lua_pushstring(L, [className UTF8String]);
    if ((error = lua_pcall(L, 1, 1, 0)))
    {
        NSLog(@"Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
    }
    if (!lua_istable(L, -1))
    {
        lua_pop(L,2);
        return nil;
    }
    if ((error = lua_pcall(L, 1, 1, 0)))
    {
        NSLog(@"Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
    }
    if (!lua_istable(L, -1))
    {
        lua_pop(L,1);
        return nil;
    }
    lua_getfield(L, -1, "rootProperty");
    if (!lua_istable(L, -1))
    {
        lua_pop(L,2);
        return nil;
    }
    PLProperty * p = [[[PLProperty alloc] initFromLua:L] autorelease];
    lua_pop(L,2);
    return p;
}

+(PrototypeController*)singleton
{
    static PrototypeController * pc = NULL;
    if (!pc)
        pc = [[PrototypeController alloc] init];
    return pc;
}

@end
