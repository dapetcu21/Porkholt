//
//  ObjectController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "ObjectController.h"
#import "PLEntity.h"

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
-(void)insertEntity:(PLEntity*)entity atIndex:(NSUInteger)index inArray:(NSUInteger)array
{
    [self insertEntities:[NSArray arrayWithObject:entity] atIndexes:[NSIndexSet indexSetWithIndex:index] inArray:array];
}

-(void)insertEntities:(NSArray*)entities atIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array
{
    [[undoManager prepareWithInvocationTarget:self] removeEntitiesAtIndexes:indexes fromArray:array];
    //TODO
    needsReindexing[array] = YES;
}

-(void)removeEntityAtIndex:(NSUInteger)index fromArray:(NSUInteger)array
{
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndex:index] fromArray:array];
}

-(void)removeEntitiesAtIndexes:(NSIndexSet*)indexes fromArray:(NSUInteger)array
{
    [[undoManager prepareWithInvocationTarget:self] insertEntities:[self entitiesForIndexes:indexes inArray:array] atIndexes:indexes inArray:array];
    //TODO
    needsReindexing[array] = YES;
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
-(void)loadFromFile:(NSURL*)file
{
    //TODO
}

-(void)saveToFile:(NSMutableString*)file
{
    //TODO
}
@end
