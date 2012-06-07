/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>

@class PLPrototype;
@class PLProperty;
@interface PrototypeController : NSObject
{
    NSMutableDictionary * props;
    lua_State * L;
}

+(PrototypeController*)singleton;
-(PLPrototype*)prototypeForClass:(NSString*)className;
-(PLProperty*)createPropertyForClass:(NSString*)className;

@end
