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

-(void)arrayChanged:(NSUInteger)array
{
    if (arrayChangeFromWithin) return;
    [[self viewForIndex:array] reloadData];
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    if (selectionChangeFromWithin) return;
    [[self viewForIndex:array] selectRowIndexes:[model selectionForArray:array] byExtendingSelection:NO];
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

#pragma mark NSTableViewDelegate

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
