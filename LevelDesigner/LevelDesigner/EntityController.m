//
//  EntityController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/21/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "EntityController.h"
#import "PLEntity.h"

@implementation EntityController
@synthesize undoManager;
@synthesize controller;

- (id)initWithArrays:(NSUInteger)array andPasteboardType:(NSString*)pbType
{
    self = [super init];
    if (self) {
        numberOfArrays = array;
        pasteboardType = [pbType retain];
        arrays = (NSMutableArray**)malloc(sizeof(NSMutableArray*)*numberOfArrays);
        selection = (NSMutableIndexSet**)malloc(sizeof(NSMutableIndexSet*)*numberOfArrays);
        needsReindexing = (BOOL*)malloc(sizeof(BOOL)*numberOfArrays);
        readOnly = (NSUInteger*)malloc(sizeof(NSUInteger)*numberOfArrays);
        arrayChanged = (BOOL*)malloc(sizeof(BOOL)*numberOfArrays);
        selectionChanged = (BOOL*)malloc(sizeof(BOOL)*numberOfArrays);
        
        NSUInteger i;
        for (i=0; i<numberOfArrays; i++)
        {
            arrays[i] = [[NSMutableArray alloc] init];
            selection[i] = [[NSMutableIndexSet alloc] init];
            needsReindexing[i] = NO;
            readOnly[i] = 0;
        }
    }
    return self;
}

-(void)dealloc
{
    NSUInteger i;
    for (i=0; i<numberOfArrays; i++)
    {
        [arrays[i] release];
        [selection[i] release];
    }
    [undoManager release];
    [pasteboardType release];
    free(arrays);
    free(selection);
    free(needsReindexing);
    free(readOnly);
    free(arrayChanged);
    free(selectionChanged);
    [super dealloc];
}

#pragma mark Array access

-(NSArray*)arrayAtIndex:(NSUInteger)array
{
    return arrays[array];
}

-(NSUInteger)readOnlyEntitiesInArray:(NSUInteger)array
{
    return readOnly[array];
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

-(void)arrayChanged:(NSUInteger)array
{
    [controller arrayChanged:array];
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [controller selectionForArrayChanged:array];
}

-(void)_arrayChanged:(NSUInteger)array
{
    if (grouping) 
    {
        arrayChanged[array] = YES;
        return;
    }
    [self arrayChanged:array];
}

-(void)_selectionForArrayChanged:(NSUInteger)array
{
    if (grouping) 
    {
        selectionChanged[array] = YES;
        return;
    }
    [self selectionForArrayChanged:array];
}

-(void)insertEntity:(PLEntity*)entity atIndex:(NSUInteger)index inArray:(NSUInteger)array
{
    [self insertEntities:[NSArray arrayWithObject:entity] atIndexes:[NSIndexSet indexSetWithIndex:index] inArray:array];
}

-(void)insertEntities:(NSArray*)entities atIndexes:(NSIndexSet*)indexes inArray:(NSUInteger)array
{
    if ([indexes count]==0) return;
    [[undoManager prepareWithInvocationTarget:self] removeEntitiesAtIndexes:indexes fromArray:array];
    NSMutableIndexSet * s = selection[array];
    __block NSInteger offset = 0;
    NSIndexSet * backup = [[s copy] autorelease];
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[entities objectAtIndex:offset];
        e.selected = NO;
        if (e.readOnly)
            readOnly[array]++;
        [s shiftIndexesStartingAtIndex:idx+offset by:1];
        offset++;
    }];
    [arrays[array] insertObjects:entities atIndexes:indexes];
    needsReindexing[array] = YES;
    [self _arrayChanged:array];
    if (![s isEqual:backup])
        [self _selectionForArrayChanged:array];
}

-(void)removeEntityAtIndex:(NSUInteger)index fromArray:(NSUInteger)array
{
    [self removeEntitiesAtIndexes:[NSIndexSet indexSetWithIndex:index] fromArray:array];
}

-(void)removeEntitiesAtIndexes:(NSIndexSet*)indexes fromArray:(NSUInteger)array
{
    if ([indexes count]==0) return;
    [[undoManager prepareWithInvocationTarget:self] insertEntities:[self entitiesForIndexes:indexes inArray:array] atIndexes:indexes inArray:array];
    NSMutableArray * a = arrays[array];
    NSMutableIndexSet * s = selection[array];
    __block NSInteger offset = 0;
    BOOL selectNext=(selectNextAfterDelete&&([s count]==1));
    NSIndexSet * backup = [[s copy] autorelease];
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[a objectAtIndex:idx];
        [e resetIndex];
        if (e.readOnly)
            readOnly[array]--;
        e.selected = NO;
        if (!selectNext || ![s containsIndex:idx-offset])
        {
            [s shiftIndexesStartingAtIndex:idx-offset+1 by:-1];
            offset++;
        }
    }];
    NSUInteger index;
    NSUInteger n = [a count]-[indexes count];
    if (selectNext && (((index=[s lastIndex])==NSNotFound) || (index>=n)))
    {
        if (index!=NSNotFound)
            [s removeIndex:index];
        if (n)
            [s addIndex:n-1];
    }
    while (((index=[s lastIndex])!=NSNotFound) && index>=n)
        [s removeIndex:index];
    [a removeObjectsAtIndexes:indexes];
    needsReindexing[array] = YES;
    [self _arrayChanged:array];
    if (![s isEqual:backup])
        [self _selectionForArrayChanged:array];
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

-(NSIndexSet*)selectionForArray:(NSUInteger)array
{
    return selection[array];
}

-(NSArray*)selectedEntitiesForArray:(NSUInteger)array
{
    return [self entitiesForIndexes:selection[array] inArray:array];
}

-(NSArray*)selectedEntities
{
    NSMutableArray * a = [[[NSMutableArray alloc] init] autorelease];
    for (int i=0; i<numberOfArrays; i++)
        [a addObjectsFromArray:[self entitiesForIndexes:selection[i] inArray:i]];
    return a;
}

-(PLEntity*)selectedEntity
{
    PLEntity * found = nil;
    for (int i=0; i<numberOfArrays; i++)
    {
        NSUInteger n = [selection[i] count];
        if (!n)
            continue;
        if (n && found)
            return nil;
        if (n==1)
            found = [arrays[i] objectAtIndex:[selection[i] lastIndex]];
        else
            return nil;
    }
    return found;
}

-(void)_insertIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array
{
    [selection[array] addIndexes:indexes];
    NSMutableArray * a = arrays[array];
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[a objectAtIndex:idx];
        e.selected = YES;
    }];
}

-(void)_removeIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array
{
    [selection[array] removeIndexes:indexes];
    NSMutableArray * a = arrays[array];
    [indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[a objectAtIndex:idx];
        e.selected = NO;
    }];
}

-(void)clearSelectionForArray:(NSUInteger)array
{
    [self _removeIndexes:selection[array] inSelectionForArray:array];
    [self _selectionForArrayChanged:array];
}

-(void)clearSelection
{
    NSUInteger i;
    for (i=0; i<numberOfArrays; i++)
        [self clearSelectionForArray:i];
}

-(void)setSelectedIndex:(NSUInteger)index forArray:(NSUInteger)array
{
    [self _removeIndexes:selection[array] inSelectionForArray:array];
    [selection[array] addIndex:index];
    ((PLEntity*)[arrays[array] objectAtIndex:index]).selected = YES;
    [self _selectionForArrayChanged:array];
}

-(void)setSelectedEntity:(PLEntity*)entity forArray:(NSUInteger)array
{
    [self setSelectedIndex:[self indexForEntity:entity inArray:array] forArray:array];
}

-(void)setSelectedIndexes:(NSIndexSet*)indexes forArray:(NSUInteger)array
{
    [self _removeIndexes:selection[array] inSelectionForArray:array];
    [self _insertIndexes:indexes inSelectionForArray:array];
    [self _selectionForArrayChanged:array];
}

-(void)setSelectedEntities:(NSArray*)entities forArray:(NSUInteger)array
{
    [self setSelectedIndexes:[self indexesForEntities:entities inArray:array] forArray:array];
}

-(void)insertIndex:(NSUInteger)index inSelectionForArray:(NSUInteger)array
{
    ((PLEntity*)[arrays[array] objectAtIndex:index]).selected = YES;
    [selection[array] addIndex:index];
    [self _selectionForArrayChanged:array];
}

-(void)removeIndex:(NSUInteger)index inSelectionForArray:(NSUInteger)array
{
    ((PLEntity*)[arrays[array] objectAtIndex:index]).selected = NO;
    [selection[array] removeIndex:index];
    [self _selectionForArrayChanged:array];
}

-(void)insertIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array
{
    [self _insertIndexes:indexes inSelectionForArray:array];
    [self _selectionForArrayChanged:array];
}

-(void)removeIndexes:(NSIndexSet*)indexes inSelectionForArray:(NSUInteger)array
{
    [self _removeIndexes:indexes inSelectionForArray:array];
    [self _selectionForArrayChanged:array];
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

#pragma mark Commit grouping

-(void)beginCommitGrouping
{
    grouping++;
}

-(void)endCommitGrouping
{
    NSUInteger i;
    if (!grouping) return;
    for (i=0; i<numberOfArrays; i++)
        if (arrayChanged[i])
        {
            arrayChanged[i] = NO;
            [self _arrayChanged:i];
        }
    for (i=0; i<numberOfArrays; i++)
        if (selectionChanged[i])
        {
            selectionChanged[i] = NO;
            [self _selectionForArrayChanged:i];
        }
    grouping--;
}

-(void)paste
{
    NSPasteboard * pb = [NSPasteboard generalPasteboard];
    NSData * d = [pb dataForType:pasteboardType];
    if (!d)
    {
        NSBeep();
        return;
    }
    [self pasteData:d atRow:-1 inArray:-1];
}

-(void)copy
{
    NSPasteboard * pb = [NSPasteboard generalPasteboard];
    [pb declareTypes:[NSArray arrayWithObject:pasteboardType] owner:self];
    [pb setData:[self copyEntitiesToData:[self selectedEntities]] forType:pasteboardType];
}

-(void)duplicate
{
    [self pasteData:[self copyEntitiesToData:[self selectedEntities]] atRow:-1 inArray:-1];
}

-(PLEntity*)newEntityForArray:(NSUInteger)array
{
    PLEntity * e = [[[[self classForArray:array] alloc] init] autorelease];
    e.owner = self;
    return e;
}

-(void)newInArray:(NSUInteger)array
{
    NSIndexSet * s = selection[array];
    NSUInteger p = [arrays[array] count];
    if ([s count]!=0)
        p = [s lastIndex]+1;
    PLEntity * e = [self newEntityForArray:array];
    [self insertEntity:e atIndex:p inArray:array];
    [self setSelectedIndex:p forArray:array];
}

-(void)delete
{
    [self deleteInArray:NSNotFound];
}

-(void)deleteInArray:(NSUInteger)carray;
{
    int array;
    NSUInteger ni=0;
    for (array = 0; array<numberOfArrays; array++)
    {
        NSMutableIndexSet * is = [[[NSMutableIndexSet alloc] init] autorelease];
        NSArray * a = arrays[array];
        [selection[array] enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
            PLEntity * e = [a objectAtIndex:idx];
            if (!e.readOnly)
                [is addIndex:idx];
        }];
        if ([is count])
        {
            if (array==carray)
                selectNextAfterDelete = YES;
            [self removeEntitiesAtIndexes:is fromArray:array];
            selectNextAfterDelete = NO;
            ni+=[is count];
        }
    }
    if (!ni)
        NSBeep();
}

-(NSData*)copyEntitiesToData:(NSArray *)entities
{
    return [NSKeyedArchiver archivedDataWithRootObject:entities];
}

-(Class)classForArray:(NSUInteger)array
{
    return nil;
}

-(void)pasteData:(NSData *)data atRow:(NSUInteger)row inArray:(NSUInteger)array
{
    NSArray * entities = [NSKeyedUnarchiver unarchiveObjectWithData:data];
    NSMutableArray * objects [numberOfArrays];
    NSUInteger i;
    for (i=0; i<numberOfArrays; i++)
        objects[i] = [[[NSMutableArray alloc] init] autorelease];
    for (PLEntity * e in entities)
        if ([e isKindOfClass:[PLEntity class]])
            for (i=0; i<numberOfArrays; i++)
                if ([e isKindOfClass:[self classForArray:i]])
                {
                    e.owner = self;
                    [objects[i] addObject:e];
                    break;
                }
    for (i=0; i<numberOfArrays; i++)
    {
        NSUInteger p = [arrays[i] count];
        if (i==array)
            p = row;
        else
            if ([selection[i] count]!=0)
                p = [selection[i] lastIndex]+1;
        [self insertEntities:objects[i] atIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(p, [objects[i] count])] inArray:i];
    }
}

@end
