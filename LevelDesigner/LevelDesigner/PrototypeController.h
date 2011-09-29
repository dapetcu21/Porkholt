//
//  PrototypeController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
