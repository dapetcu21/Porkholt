//
//  PropertyController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PropertyController.h"
#import "PLProperty.h"

@implementation PropertyController

- (id)init
{
    self = [super init];
    if (self) {
        
    }
    
    return self;
}

-(void)propertyChanged:(PLProperty*)p
{
    if (propertyChangedFromWithin) return;
    if (p==model) 
        [outlineView reloadData];
    else
        [outlineView reloadItem:p reloadChildren:YES];
}

-(void)setModel:(PLProperty*)m
{
    [model setController:nil];
    [m setController:self];
    [m retain];
    [model release];
    model = m;
    readOnlyModel = ([model.owner isKindOfClass:[PLEntity class]] && [((PLEntity*)model.owner) readOnly]);
    [self propertyChanged:model];
}

-(PLProperty*)model
{
    return model;
}

-(void)dealloc
{
    [model release];
    [super dealloc];
}

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(PLProperty*)item
{
    if (!item) item=model;
    return [item childrenCount];
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(PLProperty*)item
{
    if (!item) item=model;
    return [item propertyAtIndex:index]; 
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(PLProperty*)item
{
    if (!item) item=model;
    int type = [item type];
    return (type==PLPropertyArray)||(type==PLPropertyDictionary);
}

- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(PLProperty*)item
{
    if (!item) item=model;
    if ([[tableColumn identifier] isEqualToString:@"Key"])
        return item.name;
    //else
    if ([[tableColumn identifier] isEqualToString:@"Value"])
    {
        if (item.type==PLPropertyBoolean)
            return [NSNumber numberWithBool:item.booleanValue];
        return item.stringValue;
    }
    return nil;
}

- (void)outlineView:(NSOutlineView *)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn byItem:(PLProperty*)item
{
    if (!item) item=model;
    if ([[tableColumn identifier] isEqualToString:@"Key"])
    {
        if (item.parent.type!=PLPropertyDictionary || item.readOnly || item.mandatory)
            return;
        item.name = [object description];
    }
    else
    if ([[tableColumn identifier] isEqualToString:@"Value"])
    {
        int t = item.type;
        if (item.readOnly || t==PLPropertyDictionary || t==PLPropertyArray)
            return;
        switch (t) {
            case PLPropertyBoolean:
                item.booleanValue = [object boolValue]; 
                break;
            case PLPropertyNumber:
                item.numberValue = [object doubleValue];
                break;
            case PLPropertyString:
                item.stringValue = [object description];
                break;
            case PLPropertyPoint:
                [item setPointValueFromString:[object description]];
                break;
            case PLPropertyRect:
                [item setRectValueFromString:[object description]];
                break;
            default:
                break;
        }
    }

}

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(PLProperty*)item
{
    if (readOnlyModel) return NO;
    if (!item) item=model;
    if ([[tableColumn identifier] isEqualToString:@"Key"])
    {
        if (item.parent.type!=PLPropertyDictionary || item.readOnly || item.mandatory || item.parent.readOnly)
            return NO;
        return YES;
    }
    //else
    if ([[tableColumn identifier] isEqualToString:@"Value"])
    {
        int t = item.type;
        if (item.readOnly || t==PLPropertyDictionary || t==PLPropertyArray)
            return NO;
        return YES;
    }
    return NO;
}

-(void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(NSCell*)cell forTableColumn:(NSTableColumn *)tableColumn item:(PLProperty*)item
{
    if (!item) item=model;
    if ([[tableColumn identifier] isEqualToString:@"Key"])
    {
        [cell setEnabled:(!readOnlyModel && !item.readOnly && !item.mandatory && !item.parent.readOnly)];
    }
    else
    if ([[tableColumn identifier] isEqualToString:@"Value"])
    {
        [cell setEnabled:(!readOnlyModel && !item.readOnly)];
        if (item.type==PLPropertyBoolean)
        {
            NSButtonCell * buttonCell = (NSButtonCell*)cell;
            [buttonCell setTitle:item.stringValue];
        }
    }
}

-(NSCell*)outlineView:(NSOutlineView *)ov dataCellForTableColumn:(NSTableColumn *)tableColumn item:(PLProperty*)item
{
    if (!tableColumn) return nil;
    if (([[tableColumn identifier] isEqualToString:@"Value"]) && (item.type==PLPropertyBoolean))
        return boolCell;
    return [tableColumn dataCellForRow:[ov rowForItem:item]]; 
}


-(NSArray*)filteredPropertiesWithoutChildren:(NSArray*)props
{
    NSMutableArray * a = [NSMutableArray arrayWithCapacity:[props count]];
    for (PLProperty * p in props)
        p.mark = YES;
    for (PLProperty * p in props)
    {
        BOOL ok = YES;
        PLProperty * pp = p;
        while ((pp=pp.parent))
            if (pp.mark)
            {
                ok = NO;
                break;
            }
        if (ok)
            [a addObject:p];
    }
    for (PLProperty * p in props)
        p.mark = NO;
    return a;
}

-(NSArray*)filteredPropertiesWithoutMandatory:(NSArray *)props
{
    NSMutableArray * a = [NSMutableArray arrayWithCapacity:[props count]];
    for (PLProperty * p in props)
        if (!p.readOnly && !p.mandatory)
            [a addObject:p];
    return a;
}

-(void)new:(id)sender
{
    if (readOnlyModel) 
    {
        NSBeep();
        return;
    }
    NSInteger idx = [outlineView selectedRow];
    PLProperty * item;
    if (idx>=0)
        item = (PLProperty*)[outlineView itemAtRow:idx];
    else
        item = model;
    if ([item isCollection])
        idx = [item childrenCount];
    else
    {
        idx = item.index+1;
        item = item.parent;
    }
    PLProperty * p = [[[PLProperty alloc] initFromLua:NULL] autorelease];
    p.stringValue = @"";
    [item insertProperty:p atIndex:idx];
    [outlineView selectRowIndexes:[NSIndexSet indexSetWithIndex:[outlineView rowForItem:p]] byExtendingSelection:NO];
}

-(NSArray*)itemsAtRows:(NSIndexSet*)is
{
    NSMutableArray * a = [NSMutableArray arrayWithCapacity:[is count]];
    [is enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {
        [a addObject:[outlineView itemAtRow:idx]];
    }];
    return a;
}

-(void)delete:(id)sender
{
    if (readOnlyModel) 
    {
        NSBeep();
        return;
    }
    NSArray * items = [self filteredPropertiesWithoutChildren:[self filteredPropertiesWithoutMandatory:[self itemsAtRows:[outlineView selectedRowIndexes]]]];
    if (![items count])
    {
        NSBeep();
        return;
    }
    NSArray * sortedItems = [items sortedArrayUsingComparator:^NSComparisonResult(PLProperty * obj1, PLProperty * obj2) {
        if (obj1.parent == obj1.parent)
            return NSOrderedSame;
        return (obj1.parent < obj2.parent)?NSOrderedAscending:NSOrderedDescending;
    }];
    PLProperty * last = ((PLProperty*)[sortedItems objectAtIndex:0]).parent;
    NSUInteger lastIndex = 0;
    NSUInteger n = [sortedItems count];
    for (NSUInteger i = 1; i<=n; i++)
    {
        PLProperty * current = (i==n)?nil:((PLProperty*)[sortedItems objectAtIndex:i]).parent;
        if (last!=current)
        {
            NSArray * a = [sortedItems subarrayWithRange:NSMakeRange(lastIndex, i-lastIndex)];
            [last removeProperties:a];
        }
    }
    [outlineView selectRowIndexes:[NSIndexSet indexSet] byExtendingSelection:NO];
}


-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender
{
    return YES;
}

@end
