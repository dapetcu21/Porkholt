//
//  ObjectController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

@class InterfaceController;
@class PLEntity;
@class PLObject;
@class PLJoint;

#ifdef __cplusplus
class WorldController;
#else
#define WorldController void
#endif

#define ObjectController_numberOfArrays 2

@interface ObjectController : NSObject
{
    WorldController * worldController;
    NSMutableArray * arrays[ObjectController_numberOfArrays]; //arrays[0] = objects; arrays[1] = joints;
    NSMutableIndexSet * selection[ObjectController_numberOfArrays];
    BOOL needsReindexing[ObjectController_numberOfArrays];
    NSUndoManager * undoManager;
}

@property(nonatomic,retain) NSUndoManager * undoManager;

#pragma mark Array access
-(NSArray*)objects;
-(NSArray*)joints;
-(NSArray*)arrayAtIndex:(NSUInteger)array;

-(NSArray*)entitiesForIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array;
-(PLEntity*)entityForIndex:(NSUInteger)index inArray:(NSUInteger)array;
-(NSUInteger)indexForEntity:(PLEntity*)entity inArray:(NSUInteger)array;
-(NSIndexSet*)indexesForEntities:(NSArray*)entities inArray:(NSUInteger)array;

#pragma mark Array editing
-(void)insertEntity:(PLEntity*)entity atIndex:(NSUInteger)index inArray:(NSUInteger)array;
-(void)insertEntities:(NSArray*)entities atIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array;
-(void)removeEntityAtIndex:(NSUInteger)index fromArray:(NSUInteger)array;
-(void)removeEntitiesAtIndexes:(NSIndexSet*)indexes fromArray:(NSUInteger)array;
-(void)removeEntity:(PLEntity*)entity fromArray:(NSUInteger)array;
-(void)removeEntities:(NSArray*)entities fromArray:(NSUInteger)array;

#pragma mark Array selection
-(NSIndexSet*)selectionForArray:(NSUInteger)array;
-(void)clearSelectionForArray:(NSUInteger)array;
-(void)clearSelection;
-(void)setSelectedIndex:(NSUInteger)index forArray:(NSUInteger)array;
-(void)setSelectedEntity:(PLEntity*)entity forArray:(NSUInteger)array;
-(void)setSelectedIndexes:(NSIndexSet*)indexes forArray:(NSUInteger)array;
-(void)setSelectedEntities:(NSArray*)entities forArray:(NSUInteger)array;
-(void)insertIndex:(NSUInteger)index inSelectionForArray:(NSUInteger)array;
-(void)removeIndex:(NSUInteger)index inSelectionForArray:(NSUInteger)array;
-(void)insertIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array;
-(void)removeIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array;

-(void)insertEntity:(PLEntity*)entity inSelectionForArray:(NSUInteger)array;
-(void)removeEntity:(PLEntity*)entity inSelectionForArray:(NSUInteger)array;
-(void)insertEntities:(NSArray*)entities inSelectionForArray:(NSUInteger)array;
-(void)removeEntities:(NSArray*)entities inSelectionForArray:(NSUInteger)array;

#pragma Loading and saving to file
-(void)loadFromFile:(NSURL*)file;
-(void)saveToFile:(NSMutableString*)file;

@end
