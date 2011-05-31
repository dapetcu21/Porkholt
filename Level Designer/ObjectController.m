//
//  ObjectController.m
//  Level Designer
//
//  Created by Marius Petcu on 4/5/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ObjectController.h"
#import "PHObject.h"
#import "PHObjectProperty.h"
#import "ItemInfoTable.h"
#import "WorldController.h"
#import "MyDocument.h"
#include <stdlib.h>
#include <string.h>

#define kDragObjectPBoardType @"PHObjectPBoardType"
#define kDragPropertyPBoardType @"PHPropertyPBoardType"

#define COLUMNID_NO @"No"
#define COLUMNID_NAME @"Name"

#define COLUMNID_KEY @"Key"
#define COLUMNID_VALUE @"Value"

@implementation ObjectController

-(MyDocument*)document
{
    return document;
}

-(NSUndoManager*)undoManager
{
    return [document undoManager];
}

-(id)init
{
	if (self = [super init])
	{
		objects = [[NSMutableArray alloc] init];
	}
	return self;
}

-(void)awakeFromNib
{
	[objectBrowser registerForDraggedTypes:[NSArray arrayWithObject:kDragObjectPBoardType]];
	[itemInfo registerForDraggedTypes:[NSArray arrayWithObject:kDragPropertyPBoardType]];
	[itemInfo setKeyController:keyController];
    [itemInfo setUndoManager:[self undoManager]];
}

-(void)dealloc
{
	[objects release];
	[draggedKeys release];
	[draggedObjects release];
	[super dealloc];
}

-(void)setObjects:(NSArray*)obj
{
	if (obj==objects) return;
	[objects release];
	objects = [[NSMutableArray alloc] initWithArray:obj];
	NSLog(@"setObjects");
}

-(NSMutableArray*)objects
{
	return objects;
}

-(PHObject*)selectedObject
{
	NSArray * sel = [arrayController selectedObjects];
	if ([sel count]!=1) return nil;
	return [sel objectAtIndex:0];
}

-(void)selectObject:(PHObject*)obj
{
    if ([self selectedObject] != obj)
        [arrayController setSelectedObjects:[NSArray arrayWithObject:obj]];
}

-(NSArrayController*)arrayController
{
	return arrayController;
}

-(void)objectChanged:(PHObject*)obj
{
	if (ignoreChange) return;
	if ([self selectedObject] != obj) return;
	[itemInfo reloadData];
}

#pragma mark -
#pragma mark Saving / Loading from file

-(void)lua:(lua_State*)L setPath:(NSString*)path
{
	lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
    NSString * cur_path = [NSString stringWithUTF8String:lua_tostring( L, -1 )]; 
    cur_path = [cur_path stringByAppendingString:@";"];
    cur_path = [cur_path stringByAppendingString:path];
    lua_pop( L, 1 ); 
    lua_pushstring( L, [cur_path UTF8String]);
    lua_setfield( L, -2, "path" ); 
    lua_pop( L, 1 );
}

-(void)loadFromFile:(NSURL *)url
{
	NSMutableArray * robjs = [[[NSMutableArray alloc] init] autorelease];
	NSMutableArray * objs = [[[NSMutableArray alloc] init] autorelease];
	
	int error;
	lua_State * L = lua_open();   /* opens Lua */
	luaL_openlibs(L);
	
	[self lua:L setPath:[NSString stringWithFormat:@"%@/?.lua;%@/?.lua",[[NSBundle mainBundle] resourcePath],[url path]]];
	
	if (url)
	{
		error = luaL_loadfile(L, [[[url URLByAppendingPathComponent:@"init.lua"] path] UTF8String]) || lua_pcall(L, 0, 0, 0);
		if (error) {
			NSLog(@"Lua: %s",lua_tostring(L,-1));
			lua_pop(L, 1);
		}
	}
	error = luaL_loadfile(L, [[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"level_des.lua"] UTF8String]) || lua_pcall(L, 0, 0, 0);
	if (error) {
		NSLog(@"Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
	} 
	
	lua_getglobal(L, "objects");
	if (lua_istable(L, -1))
	{
		int n = 0;
		lua_pushstring(L, "n");
		lua_gettable(L, -2);
		if (lua_isnumber(L, -1))
			n = lua_tonumber(L, -1);
		lua_pop(L,1);
		
		for (int i=0; i<n; i++)
		{
			lua_getglobal(L, "describeObject");
			lua_pushnumber(L, i);
			lua_gettable(L, -3);
			if (lua_istable(L, -1))
			{
				error = lua_pcall(L, 1, 1, 0);
				if (error)
				{
					NSLog(@"Lua: %s",lua_tostring(L,-1));
				} else {
					if (lua_istable(L,-1))
					{
						PHObject * obj = [[[PHObject alloc] initFromLua:L] autorelease];
						obj.controller = self;
						if (obj.readOnly)
							[robjs addObject:obj];
						else
							[objs addObject:obj];
					}
				}
				lua_pop(L,1);
			} else {
				lua_pop(L,1);
				lua_pop(L,1);
			}
		}
	}
	lua_pop(L,1);
	
	lua_close(L);
	
	
	PHObject * obj = [[[PHObject alloc] init] autorelease];
	obj.readOnly = YES;
	readOnlyCells = [robjs count];
	[self setObjects:[robjs arrayByAddingObjectsFromArray:objs]];
}

-(void)saveToFile:(NSMutableString*)file
{
	[file appendString:@"--This file was generated with the Porkholt Level Designer\n"];
	[file appendString:@"--Do not modify this file. If you do, please note that this\n"];
	[file appendString:@"--file is reset every time you use the Level Designer\n"];
	for (PHObject * object in [arrayController arrangedObjects])
		if (!object.readOnly)
			[object saveToFile:file];
	[file appendString:@"\n"];
}

#pragma mark -
#pragma mark Table View Methods


-(NSCell*)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if (!tableColumn)
		return nil;
	return [tableColumn dataCellForRow:row];
}

-(void)tableView:(NSTableView *)tableView willDisplayCell:(NSCell*)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if (tableView==objectBrowser)
	{
		if (row<0) return;
		PHObject * object = [[arrayController arrangedObjects] objectAtIndex:row];
		if ([[tableColumn identifier] isEqual:COLUMNID_NO])
		{
			[cell setIntValue:row-readOnlyCells];
		}
		if ([[tableColumn identifier] isEqual:COLUMNID_NAME])
		{
			[cell setStringValue:object.className];
		}
		[cell setEnabled:[tableView isEnabled]&&!object.readOnly];	
	}
}

#pragma mark -
#pragma mark Outline View Methods 

#pragma mark -Editing

- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    if (outlineView==itemInfo)
	{
		PHObjectProperty * object = (PHObjectProperty*)[(NSTreeNode*)item representedObject];
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE])
		{
			return YES;
 		}
		if ([[tableColumn identifier] isEqual:COLUMNID_KEY])
		{
			if (object.mandatory)
				return NO;
			return YES;
		}
	}
	return NO;
}

-(void)object:(PHObject*)obj property:(PHObjectProperty*)prop changeKey:(NSString*)key
{
    [[[self undoManager] prepareWithInvocationTarget:self] object:obj property:prop changeKey:prop.key];
    prop.key = key;
    [obj modified];
}

-(void)object:(PHObject*)obj property:(PHObjectProperty*)prop changeValue:(id)value
{
    [[[self undoManager] prepareWithInvocationTarget:self] object:obj property:prop changeValue:prop.value];
    prop.value = value;
    NSString * key = prop.key;
    if ([key isEqualToString:@"class"])
    {
        [objectBrowser reloadData];
        [worldController updateSubviews];
    }
    if ([key isEqualToString:@"posX"]||
        [key isEqualToString:@"posY"]||
        [key isEqualToString:@"rotation"])
        [obj positionChanged];
    [obj modified];
}

- (void)outlineView:(NSOutlineView *)outlineView setObjectValue:(id)value forTableColumn:(NSTableColumn *)tableColumn byItem:(id)item
{
	if (outlineView==itemInfo)
	{
		ignoreChange = YES;
		PHObjectProperty * object = (PHObjectProperty*)[(NSTreeNode*)item representedObject];
        PHObject * parent = [self selectedObject];
		if ([[tableColumn identifier] isEqual:COLUMNID_KEY])
		{
			if (!object.mandatory && ![object.key isEqual:value])
			{
                [self object:parent property:object changeKey:[self proposedPropertyKey:value forSiblings:[self siblingsForItem:item] andProp:object]];
			}
		}
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE] && ![object.value isEqual:value])
		{
			[self object:parent property:object changeValue:value];
		}
		ignoreChange = NO;
	}
}

#pragma mark -Display cells

- (NSCell *)outlineView:(NSOutlineView *)outlineView dataCellForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    NSUInteger row = [outlineView rowForItem:item];
    if (outlineView==itemInfo)
	{
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE])
		{
            PHObjectProperty * object = (PHObjectProperty*)[(NSTreeNode*)item representedObject];
			if (object.type==kPHObjectPropertyBool)
				return [[boolCell copy] autorelease];
			if (object.type==kPHObjectPropertyNumber)
			{
				NSCell * cell = [[[tableColumn dataCellForRow:row] copy] autorelease];
				[cell setFormatter:numberFormatter];
				return cell;
			}
		}
	}
    return [tableColumn dataCellForRow:row];
}

- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(NSCell*)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    if (outlineView==itemInfo)
	{
		PHObjectProperty * object = (PHObjectProperty*)[(NSTreeNode*)item representedObject];
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE])
		{
            if (object.type==kPHObjectPropertyTree)
            {
                [cell setStringValue:@""];
            } else
                if (object.type==kPHObjectPropertyBool)
                {
                    [(NSButtonCell*)cell setState:object.boolValue];
                    [(NSButtonCell*)cell setTitle:object.boolValue?@"true":@"false"];
                }
                else
                {
                    [cell setStringValue:object.stringValue];
                }
		}
		if ([[tableColumn identifier] isEqual:COLUMNID_KEY])
		{
			[cell setEnabled:[outlineView isEnabled]&&!object.mandatory];
			[cell setStringValue:object.key];
		}
    }
}

#pragma mark -
#pragma mark Drag & Drop

- (BOOL)tableView:(NSTableView *)tv writeRowsWithIndexes:(NSIndexSet *)rowIndexes toPasteboard:(NSPasteboard*)pboard
{
	if (tv==objectBrowser)
	{
		[draggedObjects release];
		draggedObjects = [[NSMutableIndexSet alloc] init]; 
		[draggedObjects retain];
		NSArray * dob = [arrayController arrangedObjects];
		NSUInteger i = [rowIndexes firstIndex];
		while (i!=NSNotFound)
		{
			PHObject * obj = [dob objectAtIndex:i];
			if (!obj.readOnly)
				[(NSMutableIndexSet*)draggedObjects addIndex:i];
			i = [rowIndexes indexGreaterThanIndex:i];
		}
		NSData *data = [NSData dataWithBytes:&tv length:sizeof(id)];
		[pboard declareTypes:[NSArray arrayWithObject:kDragObjectPBoardType] owner:self];
		[pboard setData:data forType:kDragObjectPBoardType];
		return YES;
	}
	return NO;
}

- (NSDragOperation)tableView:(NSTableView*)tv validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)op
{
	if (tv==objectBrowser)
	{
		if (row>=0)
			if (op==NSTableViewDropAbove)
			{
				if ([draggedObjects count]==0)
					return NSDragOperationNone;
				if (row>=[objects count])
					return NSDragOperationEvery;
				PHObject * object = [[arrayController arrangedObjects] objectAtIndex:row];
				if (object.readOnly)
					return NSDragOperationNone;
				return NSDragOperationEvery;
			}
	}
	return NSDragOperationNone;
}

- (BOOL)tableView:(NSTableView *)aTableView acceptDrop:(id <NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation
{
	if (aTableView==objectBrowser)
	{	
		NSPasteboard* pboard = [info draggingPasteboard];
		NSData* data = [pboard dataForType:kDragObjectPBoardType];
		id obj;
		[data getBytes:&obj length:sizeof(id)];
		if (obj==objectBrowser &&draggedObjects)
		{
			NSArray * arry = [[arrayController arrangedObjects] objectsAtIndexes:draggedObjects];
            [[self undoManager] beginUndoGrouping];
            int n = [draggedObjects count];
            [self addObjects:arry atIndexSet:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(row, n)]];
			NSUInteger i = [draggedObjects firstIndex];
            NSMutableIndexSet * deleteIndexSet = [[[NSMutableIndexSet alloc] init] autorelease];
			while (i!=NSNotFound)
			{
                [deleteIndexSet addIndex:i+((i>=row)?n:0)];
                i = [draggedObjects indexGreaterThanIndex:i];
			}
            [self deleteIndexSet:deleteIndexSet];
            [[self undoManager] endUndoGrouping];
			[draggedObjects release];
			draggedObjects = nil;
			return YES;
		}
		return NO;
	}
	return NO;
}

#pragma mark -
#pragma mark Drag & Drop Properties

- (BOOL)outlineView:(NSOutlineView *)outlineView writeItems:(NSArray *)items toPasteboard:(NSPasteboard *)pboard
{
    if (outlineView==itemInfo)
    {
        [draggedKeys release];
        draggedKeys = [[NSMutableArray alloc] initWithCapacity:[items count]];
        draggingMandatory = NO;
        
        id parent = [(NSTreeNode*)[items objectAtIndex:0] parentNode];
        for (NSTreeNode* node in items)
        {
            PHObjectProperty * prop = (PHObjectProperty*)[node representedObject];
            if (prop.mandatory)
                draggingMandatory = YES;
            if ([node parentNode]!=parent)
            {
                [draggedKeys release];
                draggedKeys = nil;
                return NO;
            }
            [(NSMutableArray*)draggedKeys addObject:node];
        }
        NSData *data = [NSData dataWithBytes:&outlineView length:sizeof(id)];
        [pboard declareTypes:[NSArray arrayWithObject:kDragPropertyPBoardType] owner:self];
        [pboard setData:data forType:kDragPropertyPBoardType];
        return YES;
    }
    return NO;
}


- (NSDragOperation)outlineView:(NSOutlineView *)outlineView validateDrop:(id<NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(NSInteger)index
{
    if (outlineView==itemInfo)
    {
        if (draggingMandatory)
        {
            NSIndexPath * root = [item indexPath];
            NSIndexPath * current = [[[draggedKeys objectAtIndex:0] indexPath] indexPathByRemovingLastIndex];
            if ([root compare:current]==NSOrderedSame)
                return NSDragOperationEvery;
            return NSDragOperationNone;
        }
        PHObjectProperty * object = (PHObjectProperty*)[(NSTreeNode*)item representedObject];
        if (object.type == kPHObjectPropertyTree || item==nil)
        {
            while (item)
            {
                for (NSTreeNode * node in draggedKeys)
                    if ([node representedObject]==[item representedObject])
                        return NSDragOperationNone;
                item = [item parentNode];
            }
            return NSDragOperationEvery;
        }
    }
    return NSDragOperationNone;
}

- (BOOL)outlineView:(NSOutlineView *)outlineView acceptDrop:(id < NSDraggingInfo >)info item:(id)item childIndex:(NSInteger)index
{
    if (outlineView==itemInfo)
	{	
		NSPasteboard* pboard = [info draggingPasteboard];
		NSData* data = [pboard dataForType:kDragPropertyPBoardType];
		id obj;
		[data getBytes:&obj length:sizeof(id)];
		if (obj==itemInfo&&draggedKeys)
		{
            int cn = [draggedKeys count];
            NSMutableArray * deletePaths = [NSMutableArray arrayWithCapacity:cn];
            NSMutableArray * movedObjects = [NSMutableArray arrayWithCapacity:cn];
            for (NSTreeNode * prop in draggedKeys)
            {
                NSIndexPath * path = [prop indexPath];
                [deletePaths addObject:path];
                [movedObjects addObject:[prop representedObject]];
            }
            
            NSIndexPath * insertPath = [item indexPath];
            int n = [insertPath length];
            n++;
            NSUInteger * indexes = (NSUInteger*)malloc(sizeof(NSUInteger)*n);
            indexes[n-1]=index;
            [insertPath getIndexes:indexes];
            
            PHObjectProperty * draggedParent = [[[draggedKeys objectAtIndex:0] parentNode] representedObject];
            if ([[[draggedKeys objectAtIndex:0] indexPath] length]==1)
                                                draggedParent = nil;
            for (int i=n-1; i>=0; i--)
            {
                if ((!i && !draggedParent) || [item representedObject]==draggedParent)
                {
                    NSInteger oindex = indexes[i];
                    for (NSIndexPath* path in deletePaths)
                    {
                        if ([path indexAtPosition:[path length]-1]<oindex)
                            indexes[i]--;
                    }
                    break;
                }
                item = [item parentNode];
            }
            insertPath = [NSIndexPath indexPathWithIndexes:indexes length:n];
            free(indexes);
            
            int last = [insertPath indexAtPosition:[insertPath length]-1];
            NSMutableArray * insertPaths = [NSMutableArray arrayWithCapacity:cn];
            for (int i=0; i<cn; i++)
                [insertPaths addObject:[[insertPath indexPathByRemovingLastIndex] indexPathByAddingIndex:i+last]];
            
            //NSLog(@"delete:%@ insert:%@ move:%@",deletePaths,insertPaths,movedObjects);
            
            [[self undoManager] beginUndoGrouping];
            PHObject * obj = [self selectedObject];
            [self removePropertiesAtIndexPaths:deletePaths forObject:obj];
            [self insertProperties:movedObjects atIndexPaths:insertPaths forObject:obj];
            NSArray * sib = [self siblingsForIndexPath:insertPath];
            for (PHObjectProperty * prop in movedObjects)
                [prop setUndoable:[self undoManager] key:[self proposedPropertyKey:prop.key forSiblings:sib andProp:prop]];
            [[self undoManager] endUndoGrouping];
            
            [draggedKeys release];
			draggedKeys = nil;
			return YES;
		}
		return NO;
    }
    return NO;
}

#pragma mark -
#pragma mark Copy & Paste

-(void)addObjects:(NSArray*)arry atIndexSet:(NSIndexSet*)iSet
{
    [arrayController insertObjects:arry atArrangedObjectIndexes:iSet];
    [[self undoManager] registerUndoWithTarget:self
                                      selector:@selector(deleteIndexSet:)
                                        object:iSet];
}
     
-(void)deleteIndexSet:(NSIndexSet*)indexSet
{
    NSMutableIndexSet * nIset = [[[NSMutableIndexSet alloc] init] autorelease];
	NSUInteger i = [indexSet firstIndex];
    NSMutableArray * objcts = [[[NSMutableArray alloc] init] autorelease];
	while (i!=NSNotFound)
	{
		PHObject * obj = [[arrayController arrangedObjects] objectAtIndex:i];
		if (obj.editable)
        {
			[nIset addIndex:i];
            [objcts addObject:obj];
        }
		i = [indexSet indexGreaterThanIndex:i];
	}
	[arrayController removeObjectsAtArrangedObjectIndexes:nIset];
    [[[self undoManager] prepareWithInvocationTarget:self] addObjects:objcts atIndexSet:nIset];
}

-(IBAction)new:(id)sender
{
	NSIndexSet * indexSet = [arrayController selectionIndexes];
	NSUInteger index = [indexSet lastIndex];
	if (index==NSNotFound)
		index = [[arrayController content] count];
	else
		index++;
	int n = [objects count];
	while (index<n && ((PHObject*)[[arrayController arrangedObjects] objectAtIndex:index]).readOnly)
		index++;
	PHObject * obj = [[[PHObject alloc] init] autorelease];
	obj.controller = self;
    [self addObjects:[NSArray arrayWithObject:obj] atIndexSet:[NSIndexSet indexSetWithIndex:index]];
}


-(IBAction)delete:(id)sender
{
    [self deleteIndexSet:[[[arrayController selectionIndexes] copy] autorelease]];
}

-(IBAction)duplicate:(id)sender
{
	NSIndexSet * indexSet = [arrayController selectionIndexes];
	NSUInteger index = [indexSet lastIndex];
	if (index==NSNotFound)
		index = [[arrayController content] count];
	else
		index++;
	int n = [objects count];
	while (index<n && ((PHObject*)[[arrayController arrangedObjects] objectAtIndex:index]).readOnly)
		index++;
	NSMutableArray * duplicates = [[[NSMutableArray alloc] init] autorelease];
	NSUInteger i = [indexSet firstIndex];
	while (i!=NSNotFound)
	{
		PHObject * obj = [[[[arrayController arrangedObjects] objectAtIndex:i] copy] autorelease];
		[duplicates addObject:obj];
		i = [indexSet indexGreaterThanIndex:i];
	}
    [self addObjects:duplicates atIndexSet:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(index, [duplicates count])]];
}


-(IBAction)copy:(id)sender
{
	NSIndexSet * indexSet = [arrayController selectionIndexes];
	if ([indexSet firstIndex]==NSNotFound) return;
	NSArray * items = [[arrayController arrangedObjects] objectsAtIndexes:indexSet];
	
	NSPasteboard * cb = [NSPasteboard generalPasteboard];
	[cb declareTypes:[NSArray arrayWithObject:kObjectPBoardType] owner:self];
	[cb setData:[NSKeyedArchiver archivedDataWithRootObject:items] forType:kObjectPBoardType];
}

-(IBAction)paste:(id)sender
{
	NSPasteboard* cb = [NSPasteboard generalPasteboard];
	NSString * type = [cb availableTypeFromArray:[NSArray arrayWithObject:kObjectPBoardType]];
	if (type)
	{
		NSArray * items = [NSKeyedUnarchiver unarchiveObjectWithData:[cb dataForType:type]];
		for (PHObject * obj in items)
			obj.controller = self;
		NSIndexSet * indexSet = [arrayController selectionIndexes];
		NSUInteger index = [indexSet lastIndex];
		if (index==NSNotFound)
			index = [[arrayController content] count];
		else
			index++;
		int n = [objects count];
		while (index<n && ((PHObject*)[[arrayController arrangedObjects] objectAtIndex:index]).readOnly)
			index++;
        [self addObjects:items atIndexSet:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(index, [items count])]];
	}
}

-(BOOL)validateMenuItem:(NSMenuItem*)sender
{
	if ([[sender title] isEqual:@"Delete"]||
		[[sender title] isEqual:@"Duplicate"]||
		[[sender title] isEqual:@"Copy"])
	{
		NSIndexSet * indexSet = [arrayController selectionIndexes];
		NSUInteger index = [indexSet lastIndex];
		if (index==NSNotFound)
			return NO;
	}
	return YES;
}

#pragma mark -
#pragma mark Property Copy & Paste

-(NSArray*)siblingsForItem:(NSTreeNode*)prop
{
    NSTreeNode * par = [prop parentNode];
    if (!par)
        return [[self selectedObject] properties];
    id ro = [par representedObject];
    if ([ro isKindOfClass:[NSDictionary class]])
        return [ro valueForKey:@"childNodes"];
    return [(PHObjectProperty*)ro childNodes];
}

-(NSString*)proposedPropertyKey:(NSString*)name forSiblings:(NSArray*)arr andProp:(PHObjectProperty*)prp
{
    if (!arr) return name;
	BOOL ok;
	do
	{
		ok = YES;
		for (PHObjectProperty * prop in arr)
		{
            if (prop==prp) continue;
			if ([prop.key isEqual:name])
			{
				name = [name stringByAppendingString:@"_"];
				ok = NO;
				break;
			}
		}
	} while (!ok);
	return name;
}

-(PHObjectProperty*)propertyAtIndexPath:(NSIndexPath*)path
{
    //TODO: use arranged objects.. I think
    NSUInteger n = [path length];
    PHObjectProperty * prop = [[[self selectedObject] properties] objectAtIndex:[path indexAtPosition:0]];
    for (int i=1; i<n; i++)
        prop = [[prop childNodes] objectAtIndex:[path indexAtPosition:i]];
    return prop;
}

-(NSArray*)propertiesAtIndexPaths:(NSArray*)paths
{
    NSMutableArray * arr = [[[NSMutableArray alloc] initWithCapacity:[paths count]] autorelease];
    for (NSIndexPath * path in paths)
        [arr addObject:[self propertyAtIndexPath:path]];
    return arr;
}

-(NSArray*)siblingsForIndexPath:(NSIndexPath*)path
{
    if ([path length]<=1)
        return [[self selectedObject] properties];
    return [[self propertyAtIndexPath:[path indexPathByRemovingLastIndex]] childNodes];
}

-(NSIndexPath*)propInsertPositionForSelection:(NSArray*)indexPaths
{
    NSUInteger minL = 0x3f3f3f3f;
    NSIndexPath * last = nil;
    for (NSIndexPath* path in indexPaths)
    {
        NSUInteger l = [path length];
        if (l<minL)
            minL = l;
        if (l==minL)
            last = path;
    }
    if (!last)
        return [NSIndexPath indexPathWithIndex:[[[self selectedObject] properties] count]];
    PHObjectProperty * prop = [self propertyAtIndexPath:last];
    if (prop.type == kPHObjectPropertyTree)
    {
        return [last indexPathByAddingIndex:[[prop childNodes] count]];
    }
    NSUInteger ix = [last indexAtPosition:[last length]-1];
    ix++;
    return [[last indexPathByRemovingLastIndex] indexPathByAddingIndex:ix];
}

-(BOOL)indexPath:(NSIndexPath*)p1 includedIn:(NSIndexPath*)p2
{
    int n1 = [p1 length];
    int n2 = [p2 length];
    if (n1<n2) return NO;
        for (int i=0; i<n2; i++)
            if ([p1 indexAtPosition:i]!=[p2 indexAtPosition:i])
                return NO;
    return YES;
}

-(NSArray*)parentFilteredIndexPaths:(NSArray*)paths
{
    NSUInteger n = [paths count];
    if (!n) return [NSArray array];
    BOOL * bif = malloc(sizeof(BOOL)*n);
    memset(bif,0,sizeof(BOOL)*n);
    int i,j,nc;
    nc=n;
    for (i=0; i<n; i++)
    {
        NSIndexPath * p1 = [paths objectAtIndex:i];
        for (j=0; j<i; j++)
            if (!bif[j])
            {
                NSIndexPath * p2 = [paths objectAtIndex:j];
                if ([self indexPath:p1 includedIn:p2])
                {
                    bif[i]=YES;
                    nc--;
                    break;
                }
                if ([self indexPath:p2 includedIn:p1])
                {
                    bif[j]=YES;
                    nc--;
                }
            }
    }
    NSMutableArray * arr = [[[NSMutableArray alloc] initWithCapacity:nc] autorelease];
    for (int i=0; i<n; i++)
        if (!bif[i])
            [arr addObject:[paths objectAtIndex:i]];
    free(bif);
    return arr;
}

-(void)removePropertiesAtIndexPaths:(NSArray*)paths forObject:(PHObject*)obj
{
    [self selectObject:obj];
    [[[self undoManager] prepareWithInvocationTarget:self] insertProperties:[self propertiesAtIndexPaths:paths] atIndexPaths:paths forObject:obj];
    [keyController removeObjectsAtArrangedObjectIndexPaths:paths];
    [keyController setSelectionIndexPaths:[NSArray array]];
}

-(void)insertProperties:(NSArray*)props atIndexPaths:(NSArray*)paths forObject:(PHObject*)obj
{
    [self selectObject:obj];
    [[[self undoManager] prepareWithInvocationTarget:self] removePropertiesAtIndexPaths:paths forObject:obj];
    [keyController insertObjects:props atArrangedObjectIndexPaths:paths];
    [keyController setSelectionIndexPaths:paths];
}

-(IBAction)newProp:(id)sender
{
	if (![self selectedObject]) return;
	NSIndexPath * path = [self propInsertPositionForSelection:[keyController selectionIndexPaths]];
    PHObjectProperty * prop = [PHObjectProperty propertyWithValue:@"" ofType:kPHObjectPropertyString forKey:[self proposedPropertyKey:@"untitled" forSiblings:[self siblingsForIndexPath:path] andProp:nil]];
    [self insertProperties:[NSArray arrayWithObject:prop] atIndexPaths:[NSArray arrayWithObject:path] forObject:[self selectedObject]];
}


-(IBAction)deleteProp:(id)sender
{
	if (![self selectedObject]) return;
	NSArray * arry = [self parentFilteredIndexPaths:[keyController selectionIndexPaths]];
    NSMutableArray * arr = [[[NSMutableArray alloc] initWithCapacity:[arry count]] autorelease];
    for (NSIndexPath * path in arry)
    {
        PHObjectProperty * prop = [self propertyAtIndexPath:path];
        if (!prop.mandatory)
            [arr addObject:path];
    }
    [self removePropertiesAtIndexPaths:arr forObject:[self selectedObject]];
}

-(IBAction)duplicateProp:(id)sender
{
    if (![self selectedObject]) return;
    NSArray * arr = [keyController selectionIndexPaths];
    NSMutableArray * indexPaths = [NSMutableArray arrayWithCapacity:[arr count]];
    [indexPaths addObjectsFromArray:arr];
    NSMutableArray * objcts = [NSMutableArray arrayWithCapacity:[indexPaths count]];
    
    NSMutableArray * rootIP = [NSMutableArray arrayWithCapacity:[indexPaths count]];
    int * indexes = (int*)malloc([indexPaths count]*sizeof(int));
    
    int j = 0;
    int n = [indexPaths count];
    for (j=0; j<n; j++)
    {
        NSIndexPath * path = [indexPaths objectAtIndex:j];
        PHObjectProperty * oprop = [self propertyAtIndexPath:path];
        PHObjectProperty * prop = [oprop copy];
        [prop setMandatory:NO];
        NSArray * sib = [self siblingsForIndexPath:path];
        prop.key = [self proposedPropertyKey:prop.key forSiblings:sib andProp:prop];
        [objcts addObject:prop];
        NSIndexPath * p = [path indexPathByRemovingLastIndex];
        int i = 0;
        for (NSIndexPath * rp in rootIP)
        {
            if ([p compare:rp]==NSOrderedSame)
                break;
            i++;
        }
        if (i==[rootIP count])
        {
            indexes[i]=[sib count];
            [rootIP addObject:p];
        }
        p = [p indexPathByAddingIndex:indexes[i]];
        [indexPaths replaceObjectAtIndex:j withObject:p];
        indexes[i]++;
    }
    free(indexes);
    [self insertProperties:objcts atIndexPaths:indexPaths forObject:[self selectedObject]];
}


-(IBAction)copyProp:(id)sender
{
	if (![self selectedObject]) return;
	NSArray * indexPaths = [keyController selectionIndexPaths];
    if ([indexPaths count]==0) return;
	NSPasteboard * cb = [NSPasteboard generalPasteboard];
	NSArray * items = [self propertiesAtIndexPaths:indexPaths];
	[cb declareTypes:[NSArray arrayWithObject:kPropertyPBoardType] owner:self];
	[cb setData:[NSKeyedArchiver archivedDataWithRootObject:items] forType:kPropertyPBoardType];
}

-(IBAction)pasteProp:(id)sender
{
	NSPasteboard* cb = [NSPasteboard generalPasteboard];
	NSString * type = [cb availableTypeFromArray:[NSArray arrayWithObject:kPropertyPBoardType]];
	if (type)
	{
		NSArray * items = [NSKeyedUnarchiver unarchiveObjectWithData:[cb dataForType:type]];
		
        NSIndexPath * path = [self propInsertPositionForSelection:[keyController selectionIndexPaths]];
        NSIndexPath * base = [path indexPathByRemovingLastIndex];
        int n = [items count];
        int s = [path indexAtPosition:[path length]-1];
        NSMutableArray * ipaths = [NSMutableArray arrayWithCapacity:n];
        for (int i=s; i<s+n; i++)
            [ipaths addObject:[base indexPathByAddingIndex:i]];
        NSArray * sib = [self siblingsForIndexPath:path];
        
        for (PHObjectProperty * obj in items)
		{
			obj.mandatory = NO;
			obj.key = [self proposedPropertyKey:obj.key forSiblings:sib andProp:obj];
		}
		
      	[self insertProperties:items atIndexPaths:ipaths forObject:[self selectedObject]];
	}
}

-(BOOL)validateMenuItemProp:(NSMenuItem*)sender
{
    NSString * title = [sender title];
	if ([title isEqualToString:@"Delete"]||
        [title isEqualToString:@"Duplicate"]||
		[title isEqualToString:@"Copy"])
	{
		NSUInteger n = [[keyController selectionIndexPaths] count];
		if (n==0)
			return NO;
	}
	return YES;
}

@end
