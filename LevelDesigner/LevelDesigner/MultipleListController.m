//
//  MultipleListController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "MultipleListController.h"
#import "EntityController.h"
#import "PLEntity.h"
#import "PLTableView.h"

@implementation MultipleListController

-(void)setModel:(EntityController*)m
{
    [model setController:nil];
    [m setController:self];
    [m retain];
    [model release];
    model = m;
    for (NSUInteger i=0; i<numberOfTables; i++)
    {
        [self arrayChanged:i];
        [self selectionForArrayChanged:i];
    }
}

-(EntityController*)model
{
    return model;
}


-(id)           initWithTables:(NSUInteger)array 
             andPasteboardType:(NSString*)pbType
     andLocationPasteboardType:(NSString*)lpbType
      andPointerPasteboardType:(NSString*)ppbType;
{
    self = [super init];
    if (self) {
        numberOfTables = array;
        pasteboardType = pbType;
        locationPasteboardType = lpbType;
        pointerPasteboardType = ppbType;
        tables = (PLTableView**)malloc(sizeof(PLTableView*)*numberOfTables);
        memset(tables,0,sizeof(PLTableView*)*numberOfTables);
    }
    
    return self;
}

-(void)dealloc
{
    [model setController:nil];
    [model release];
    for (int i=0; i<numberOfTables; i++)
        [tables[i] release];
    free(tables);
    [super dealloc];
}


- (void)awakeFromNib
{
    for (int i=0; i<numberOfTables; i++)
        [tables[i] registerForDraggedTypes:[NSArray arrayWithObjects:pasteboardType,locationPasteboardType,pointerPasteboardType,nil]];
}

-(NSUInteger)indexForView:(id)sender
{
    for (int i=0; i<numberOfTables; i++)
        if (sender==tables[i])
            return i;
    return NSNotFound;
}

-(void)_switchTabs
{
    int current = [[[tabView selectedTabViewItem] identifier] intValue];
    if (tabToMoveTo!=-1 && tabToMoveTo!=current)
        [tabView selectTabViewItemWithIdentifier:[NSString stringWithFormat:@"%d",tabToMoveTo]];
}

-(void)switchTabs
{
    tabToMoveTo = -1;
    int i;
    BOOL done = NO;
    for (i=0; i<numberOfTables; i++)
        if ([[model selectionForArray:i] count]!=0)
        {
            if (done)
            {
                tabToMoveTo=-1;
                break;
            } else
                tabToMoveTo = i;
        }
    [self performSelectorOnMainThread:@selector(_switchTabs) withObject:nil waitUntilDone:NO];
}

-(void)arrayChanged:(NSUInteger)array
{
    if (arrayChangeFromWithin) return;
    selectionChangeFromWithin = YES;
    [tables[array] reloadData];
    selectionChangeFromWithin = NO;
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    if (selectionChangeFromWithin) return;
    selectionChangeFromWithin = YES;
    NSTableView * tv = tables[array];
    NSIndexSet * is = [model selectionForArray:array];
    [tv selectRowIndexes:is byExtendingSelection:NO];
    if ([is count]!=0)
        [tv scrollRowToVisible:[is lastIndex]];
    [self switchTabs];
    selectionChangeFromWithin = NO;
}


#pragma mark NSTableViewDataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return [[model arrayAtIndex:[self indexForView:tableView]] count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return nil;
    PLEntity * entity = [[model arrayAtIndex:array] objectAtIndex:row];
    id columnId = [tableColumn identifier];
    if ([columnId isEqualToString:@"Index"])
        return [NSNumber numberWithInt:(int)row-(int)[model numberOfReadOnlyEntitiesInArray:array]];
    if ([columnId isEqualToString:@"Name"])
        return [entity description];
    return nil;
}

- (BOOL)tableView:(NSTableView *)aTableView shouldEditTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    return NO;
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(NSCell*)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return;
    PLEntity * entity = [[model arrayAtIndex:array] objectAtIndex:row];
    [cell setEnabled:(!model.readOnly && !entity.readOnly)];
}

- (BOOL)tableView:(NSTableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pboard
{
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return NO;
    NSArray * a = [model arrayAtIndex:array];
    __block BOOL ret = YES;
    [rowIndexes enumerateIndexesUsingBlock: ^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[a objectAtIndex:idx];
        if (e.readOnly) { ret = NO; *stop = YES; }
    }];
    if (!ret) return NO;
    
    NSData * pos = [NSKeyedArchiver archivedDataWithRootObject:rowIndexes];
    id pnt = tableView;
    NSData * pointer = [NSData dataWithBytes:&pnt length:sizeof(id)];
    [pboard declareTypes:[NSArray arrayWithObjects:pasteboardType,locationPasteboardType,pointerPasteboardType,nil ] owner:self];
    [pboard setData:pos forType:locationPasteboardType];
    [pboard setData:pointer forType:pointerPasteboardType];
    [pboard setData:[model copyEntitiesToData:[model entitiesForIndexes:rowIndexes inArray:array]] forType:pasteboardType];
    return YES;
}

- (NSDragOperation)tableView:(NSTableView*)tableView validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)operation
{
    if (operation==NSTableViewDropOn) 
        return NSDragOperationNone;
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return NSDragOperationNone;
    if (row<[model numberOfReadOnlyEntitiesInArray:array]) 
        return NSDragOperationNone;
    return NSDragOperationEvery;
}

- (BOOL)tableView:(NSTableView *)tableView acceptDrop:(id < NSDraggingInfo >)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation
{
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return NO;
    if (row<[model numberOfReadOnlyEntitiesInArray:array])
        return NO;
    NSPasteboard * pb = [info draggingPasteboard];
    NSData * dt;
    dt = [pb dataForType:pointerPasteboardType];
    if ( dt && [dt length]>=sizeof(id) && (((id*)[dt bytes])[0]==tableView) )
    {
        dt = [pb dataForType:locationPasteboardType];
        NSIndexSet * indexSet = (NSIndexSet*)[NSKeyedUnarchiver unarchiveObjectWithData:dt];
        if (![indexSet isKindOfClass:[NSIndexSet class]])
            return NO;
        __block NSUInteger r = row;
        [indexSet enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
            if (idx<row)
                r--;
            else
                *stop = YES;
        }];
        NSArray * e = [model entitiesForIndexes:indexSet inArray:array];
        [model removeEntitiesAtIndexes:indexSet fromArray:array];
        NSIndexSet * newIndexSet = [NSIndexSet indexSetWithIndexesInRange:NSMakeRange(r, [e count])];
        [model insertEntities:e atIndexes:newIndexSet inArray:array];
        [model setSelectedIndexes:newIndexSet forArray:array];
        return YES;
    } 
    dt = [pb dataForType:pasteboardType];
    if (dt)
    {
        [model pasteData:dt atRow:row inArray:array];
        return YES;
    }
    return NO;
}



#pragma mark NSTableViewDelegate

- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if (selectionChangeFromWithin) return;
    NSTableView * tv = (NSTableView*)[aNotification object];
    NSUInteger array = [self indexForView:tv];
    if (array==NSNotFound) return;
    selectionChangeFromWithin = YES;
    [model setSelectedIndexes:[tv selectedRowIndexes] forArray:array];
    if (([NSEvent modifierFlags] & (NSShiftKeyMask | NSCommandKeyMask))==0)
    {
        int i;
        for (i=0; i<numberOfTables; i++)
            if (i!=array)
            {
                [model setSelectedIndexes:[NSIndexSet indexSet] forArray:i];
                [tables[i] selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];
            }
    }
    selectionChangeFromWithin = NO;
}


#pragma mark copy paste
-(void)new:(id)sender
{
    [model newInArray:[self indexForView:sender]];
}

-(void)delete:(id)sender
{
    [model deleteInArray:[self indexForView:sender]];
}

-(void)copy:(id)sender
{
    [model copyToPasteboard];
}

-(void)paste:(id)sender
{
    [model paste];
}

-(void)duplicate:(id)sender
{
    [model duplicate];
}

-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender
{
    return YES;
}


@end
