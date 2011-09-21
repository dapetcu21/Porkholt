//
//  InterfaceController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "InterfaceController.h"
#import "ObjectController.h"
#import "PLTableView.h"
#import "PLEntity.h"

@implementation InterfaceController

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (void)awakeFromNib
{
    [objectView registerForDraggedTypes:[NSArray arrayWithObjects:PLObjectPBoardType,PLObjectLocationPBoardType,PLObjectPointerPBoardType,nil]];
    [jointView registerForDraggedTypes:[NSArray arrayWithObjects:PLObjectPBoardType,PLObjectLocationPBoardType,PLObjectPointerPBoardType,nil]];
}

-(NSUInteger)indexForView:(id)sender
{
    if (sender==objectView)
        return 0;
    if (sender==jointView)
        return 1;
    return NSNotFound;
}

-(PLTableView*)viewForIndex:(NSUInteger)i
{
    switch(i)
    {
        case 0:
            return objectView;
        case 1:
            return jointView;
        default:
            return nil;
    }
}

-(void)switchTabs
{
    int current = [[[tabView selectedTabViewItem] identifier] intValue];
    int tab=current;
    int i;
    BOOL done = NO;
    for (i=0; i<ObjectController_numberOfArrays; i++)
        if ([[model selectionForArray:i] count]!=0)
        {
            if (done)
            {
                tab=current;
                break;
            } else
                tab = i;
        }
    if (tab!=current)
        [tabView selectTabViewItemWithIdentifier:[NSString stringWithFormat:@"%d",tab]];
}

-(void)arrayChanged:(NSUInteger)array
{
    if (arrayChangeFromWithin) return;
    selectionChangeFromWithin = YES;
    [[self viewForIndex:array] reloadData];
    selectionChangeFromWithin = NO;
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    if (selectionChangeFromWithin) return;
    selectionChangeFromWithin = YES;
    NSTableView * tv = [self viewForIndex:array];
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
        return [NSNumber numberWithInt:(int)row-(int)[model readOnlyEntitiesInArray:array]];
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
    [cell setEnabled:!entity.readOnly];
}

- (BOOL)tableView:(NSTableView *)tableView writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard *)pboard
{
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return NO;
    NSArray * a = [model arrayAtIndex:array];
    __block BOOL ret = YES;
    [rowIndexes enumerateIndexesUsingBlock: ^(NSUInteger idx, BOOL *stop) {
        PLEntity * e = (PLEntity*)[a objectAtIndex:idx];
        if (e.readOnly) { ret = NO; *stop = NO; }
    }];
    if (!ret) return NO;
    
    NSData * pos = [NSKeyedArchiver archivedDataWithRootObject:rowIndexes];
    id pnt = tableView;
    NSData * pointer = [NSData dataWithBytes:&pnt length:sizeof(id)];
    [pboard declareTypes:[NSArray arrayWithObjects:PLObjectPBoardType,PLObjectLocationPBoardType,PLObjectPointerPBoardType,PLObjectPointerPBoardType,nil ] owner:self];
    [pboard setData:pos forType:PLObjectLocationPBoardType];
    [pboard setData:pointer forType:PLObjectPointerPBoardType];
    [pboard setData:[model copyEntitiesToData:[model entitiesForIndexes:rowIndexes inArray:array]] forType:PLObjectPBoardType];
    return YES;
}

- (NSDragOperation)tableView:(NSTableView*)tableView validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)operation
{
    if (operation==NSTableViewDropOn) 
        return NSDragOperationNone;
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return NSDragOperationNone;
    if (row<[model readOnlyEntitiesInArray:array]) 
        return NSDragOperationNone;
    return NSDragOperationEvery;
}

- (BOOL)tableView:(NSTableView *)tableView acceptDrop:(id < NSDraggingInfo >)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation
{
    NSUInteger array = [self indexForView:tableView];
    if (array==NSNotFound) return NO;
    if (row<[model readOnlyEntitiesInArray:array])
        return NO;
    NSPasteboard * pb = [info draggingPasteboard];
    NSData * dt;
    dt = [pb dataForType:PLObjectPointerPBoardType];
    if ( dt && [dt length]>=sizeof(id) && (((id*)[dt bytes])[0]==tableView) )
    {
        dt = [pb dataForType:PLObjectLocationPBoardType];
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
    dt = [pb dataForType:PLObjectPBoardType];
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
        for (i=0; i<ObjectController_numberOfArrays; i++)
            if (i!=array)
            {
                [model setSelectedIndexes:[NSIndexSet indexSet] forArray:i];
                [[self viewForIndex:i] selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];
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
    [model delete];
}

-(void)copy:(id)sender
{
    [model copy];
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
