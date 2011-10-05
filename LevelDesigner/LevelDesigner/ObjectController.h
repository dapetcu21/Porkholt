//
//  ObjectController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "EntityController.h"

@class InterfaceController;
@class WorldController;

#define ObjectController_numberOfArrays 2
#define PLObjectPBoardType @"PLObjectPBoardType"
#define PLObjectLocationPBoardType @"PLObjectLocationPBoardType"
#define PLObjectPointerPBoardType @"PLObjectPointerPBoardType"

@interface ObjectController : EntityController
{
    IBOutlet WorldController * worldController;
}

#pragma Array Access
-(NSArray*)objects;
-(NSArray*)joints;

#pragma mark Loading and saving to file
-(void)loadFromFile:(NSURL*)file;
-(void)saveToFile:(NSMutableString*)file;

@end
