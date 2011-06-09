//
//  InheritanceController.h
//  Level Designer
//
//  Created by Marius Petcu on 6/8/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Foundation/Foundation.h>

@class PHObject;

@interface InheritanceController : NSObject {
@private
    lua_State * L;
    NSMutableDictionary * models;
}

- (PHObject*)createModelForClass:(NSString*)class;
- (PHObject*)modelForClass:(NSString*)class;

+ (InheritanceController*)sharedInstance;

@end
