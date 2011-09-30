//
//  EntityController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/21/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
@class PLEntity;
@class MultipleListController;

@interface EntityController : NSObject
{
    NSUndoManager * undoManager;
    NSUInteger grouping;
    NSString * pasteboardType;
    NSUInteger numberOfArrays;
    NSMutableArray ** arrays;
    NSMutableIndexSet ** selection;
    BOOL * needsReindexing;
    NSUInteger * readOnly;
    BOOL * arrayChanged;
    BOOL * selectionChanged;
    BOOL selectNextAfterDelete;
    BOOL ro;
}

-(id)initWithArrays:(NSUInteger)array andPasteboardType:(NSString*)pbType;

@property(nonatomic,retain) NSUndoManager * undoManager;
@property(nonatomic,assign) IBOutlet MultipleListController * controller;
@property(nonatomic,assign) BOOL readOnly;

#pragma mark Array access
-(PLEntity*)selectedEntity;
-(NSArray*)arrayAtIndex:(NSUInteger)array;
-(NSUInteger)numberOfReadOnlyEntitiesInArray:(NSUInteger)array;

-(NSArray*)entitiesForIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array;
-(PLEntity*)entityForIndex:(NSUInteger)index inArray:(NSUInteger)array;
-(NSUInteger)indexForEntity:(PLEntity*)entity inArray:(NSUInteger)array;
-(NSIndexSet*)indexesForEntities:(NSArray*)entities inArray:(NSUInteger)array;

#pragma mark Commit grouping
-(void)beginCommitGrouping;
-(void)endCommitGrouping;

#pragma mark Array editing
-(NSArray*)readWriteEntitiesInArray:(NSUInteger)array;
-(NSArray*)readOnlyEntitiesInArray:(NSUInteger)array;
-(void)insertEntity:(PLEntity*)entity atIndex:(NSUInteger)index inArray:(NSUInteger)array;
-(void)insertEntities:(NSArray*)entities atIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array;
-(void)insertEntities:(NSArray*)entities atIndex:(NSUInteger)index inArray:(NSUInteger)array;
-(void)removeEntityAtIndex:(NSUInteger)index fromArray:(NSUInteger)array;
-(void)removeEntitiesAtIndexes:(NSIndexSet*)indexes fromArray:(NSUInteger)array;
-(void)removeEntity:(PLEntity*)entity fromArray:(NSUInteger)array;
-(void)removeEntities:(NSArray*)entities fromArray:(NSUInteger)array;

#pragma mark Array selection
-(NSIndexSet*)selectionForArray:(NSUInteger)array;
-(NSArray*)selectedEntitiesForArray:(NSUInteger)array;
-(NSArray*)selectedEntities;
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

#pragma mark Copy paste and Misc
-(Class)classForArray:(NSUInteger)array;
-(PLEntity*)newEntityForArray:(NSUInteger)array;
-(void)newInArray:(NSUInteger)array;
-(void)delete;
-(void)deleteInArray:(NSUInteger)array;
-(void)copyToPasteboard;
-(NSData*)copyEntitiesToData:(NSArray*)entities;
-(void)paste;
-(void)pasteData:(NSData*)data atRow:(NSUInteger)row inArray:(NSUInteger)array;
-(void)duplicate;

-(void)entityDescriptionChanged:(PLEntity*)e;

@end
