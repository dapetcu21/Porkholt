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
@class StatusController;
@class PLDocument;

#define ObjectController_numberOfArrays 3
#define PLObjectPBoardType @"PLObjectPBoardType"
#define PLObjectLocationPBoardType @"PLObjectLocationPBoardType"
#define PLObjectPointerPBoardType @"PLObjectPointerPBoardType"

@interface ObjectController : EntityController
{
    IBOutlet WorldController * worldController;
    IBOutlet StatusController * statusController;
    BOOL showMarkers, showImages, showFixtures, showJoints;
    BOOL objectMode;
    IBOutlet PLDocument * document;
}

#pragma Array Access
-(NSArray*)objects;
-(NSArray*)joints;

#pragma mark Loading and saving to file
-(void)loadFromFile:(NSURL*)file;
-(void)saveToFile:(NSMutableString*)file;

@property(nonatomic,assign) BOOL showMarkers;
@property(nonatomic,assign) BOOL showImages;
@property(nonatomic,assign) BOOL showFixtures;
@property(nonatomic,assign) BOOL showJoints;
@property(nonatomic,assign) BOOL objectMode;
@property(nonatomic,readonly,getter = isObjectModePossible) BOOL objectModeEnabled;
-(BOOL)isObjectModePossible;
-(NSURL*)fileURL;

@end
