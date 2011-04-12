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

#define kDragObjectPBoardType @"PHObjectPBoardType"
#define kDragPropertyPBoardType @"PHPropertyPBoardType"

#define COLUMNID_NO @"No"
#define COLUMNID_NAME @"Name"

#define COLUMNID_KEY @"Key"
#define COLUMNID_VALUE @"Value"

@implementation ObjectController

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
}

-(NSMutableArray*)objects
{
	return objects;
}

-(PHObject*)selectedObject
{
	return [arrayController selection];
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
#pragma mark Table view methods

-(BOOL)tableView:(NSTableView *)tableView shouldEditTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if (tableView==itemInfo)
	{
		if (row<0) return NO;
		PHObjectProperty * object = [[keyController arrangedObjects] objectAtIndex:row];
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

-(void)tableView:(NSTableView *)tableView setObjectValue:(id)value forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if (tableView==itemInfo)
	{
		if (row<0) return;
		PHObjectProperty * object = [[keyController arrangedObjects] objectAtIndex:row];
		if ([[tableColumn identifier] isEqual:COLUMNID_KEY])
		{
			if (!object.mandatory && ![object.key isEqual:value])
				object.key = [self proposedPropertyKey:value];
		}
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE])
		{
			object.value = value;
			if ([object.key isEqual:@"class"])
				[objectBrowser reloadData];
		}
	}
}

-(NSCell*)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
	if (!tableColumn)
		return nil;
	if (tableView==itemInfo)
	{
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE])
		{
			PHObjectProperty * object = [[keyController arrangedObjects] objectAtIndex:row];
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
	if (tableView==itemInfo)
	{
		if (row<0) return;
		PHObjectProperty * object = [[keyController arrangedObjects] objectAtIndex:row];
		if ([[tableColumn identifier] isEqual:COLUMNID_VALUE])
		{
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
			[cell setEnabled:[tableView isEnabled]&&!object.mandatory];
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
	if (tv==itemInfo)
	{
		[draggedKeys release];
		draggedKeys = [rowIndexes copy];
		NSData *data = [NSData dataWithBytes:&tv length:sizeof(id)];
		[pboard declareTypes:[NSArray arrayWithObject:kDragPropertyPBoardType] owner:self];
		[pboard setData:data forType:kDragPropertyPBoardType];
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
	if (tv==itemInfo)
	{
		if (row>=0)
			if (op==NSTableViewDropAbove)
				return NSDragOperationEvery;
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
			[arrayController removeObjectsAtArrangedObjectIndexes:draggedObjects];
			NSUInteger i = [draggedObjects firstIndex];
			NSUInteger or = row;
			while (i!=NSNotFound && i<or)
			{
				row--;
				i = [draggedObjects indexGreaterThanIndex:i];
			}
			int n = [draggedObjects count];
			[arrayController insertObjects:arry atArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(row, n)]];
			[draggedObjects release];
			draggedObjects = nil;
			return YES;
		}
		return NO;
	}
	if (aTableView==itemInfo)
	{	
		NSPasteboard* pboard = [info draggingPasteboard];
		NSData* data = [pboard dataForType:kDragPropertyPBoardType];
		id obj;
		[data getBytes:&obj length:sizeof(id)];
		if (obj==itemInfo &&draggedKeys)
		{
			NSArray * arry = [[keyController arrangedObjects] objectsAtIndexes:draggedKeys];
			[keyController removeObjectsAtArrangedObjectIndexes:draggedKeys];
			NSUInteger i = [draggedKeys firstIndex];
			NSUInteger or = row;
			while (i!=NSNotFound && i<or)
			{
				row--;
				i = [draggedKeys indexGreaterThanIndex:i];
			}
			int n = [draggedKeys count];
			[keyController insertObjects:arry atArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(row, n)]];
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
	[arrayController insertObject:[[[PHObject alloc] init] autorelease] atArrangedObjectIndex:index];
}

-(IBAction)delete:(id)sender
{
	NSIndexSet * indexSet = [[[arrayController selectionIndexes] copy] autorelease];
	NSMutableIndexSet * nIset = [[[NSMutableIndexSet alloc] init] autorelease];
	NSUInteger i = [indexSet firstIndex];
	while (i!=NSNotFound)
	{
		PHObject * obj = [[arrayController arrangedObjects] objectAtIndex:i];
		if (obj.editable)
			[nIset addIndex:i];
		i = [indexSet indexGreaterThanIndex:i];
	}
	[arrayController removeObjectsAtArrangedObjectIndexes:nIset];
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
	[arrayController insertObjects:duplicates atArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(index, [duplicates count])]];
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
		NSIndexSet * indexSet = [arrayController selectionIndexes];
		NSUInteger index = [indexSet lastIndex];
		if (index==NSNotFound)
			index = [[arrayController content] count];
		else
			index++;
		int n = [objects count];
		while (index<n && ((PHObject*)[[arrayController arrangedObjects] objectAtIndex:index]).readOnly)
			index++;
		[arrayController insertObjects:items atArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(index, [items count])]];
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

-(NSString*)proposedPropertyKey:(NSString*)name
{
	NSArray * arr = [keyController arrangedObjects];
	BOOL ok;
	do
	{
		ok = YES;
		for (PHObjectProperty * prop in arr)
		{
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

-(IBAction)newProp:(id)sender
{
	if (![self selectedObject]) return;
	NSIndexSet * indexSet = [keyController selectionIndexes];
	NSUInteger index = [indexSet lastIndex];
	if (index==NSNotFound)
		index = [[keyController content] count];
	else
		index++;
	PHObjectProperty * prop = [PHObjectProperty propertyWithValue:@"" ofType:kPHObjectPropertyString forKey:[self proposedPropertyKey:@"untitled"]];
	[keyController insertObject:prop atArrangedObjectIndex:index];
}

-(IBAction)deleteProp:(id)sender
{
	if (![self selectedObject]) return;
	NSMutableIndexSet * indexSet = [[[NSMutableIndexSet alloc] initWithIndexSet:[keyController selectionIndexes]]autorelease];
	NSUInteger i = [indexSet firstIndex];
	NSArray * arry = [keyController arrangedObjects];
	while (i!=NSNotFound)
	{
		if (((PHObjectProperty *)[arry objectAtIndex:i]).mandatory)
			[indexSet removeIndex:i];
		i = [indexSet indexGreaterThanIndex:i];
	}
	[keyController removeObjectsAtArrangedObjectIndexes:indexSet];
}

-(IBAction)duplicateProp:(id)sender
{
	if (![self selectedObject]) return;
	NSIndexSet * indexSet = [keyController selectionIndexes];
	NSUInteger index = [indexSet lastIndex];
	if (index==NSNotFound)
		index = [[keyController content] count];
	else
		index++;
	NSMutableArray * objcts = [[NSMutableArray alloc] initWithCapacity:[indexSet count]];
	NSUInteger i = [indexSet firstIndex];
	NSArray * arry = [keyController arrangedObjects];
	while (i!=NSNotFound)
	{
		PHObjectProperty * obj = [[[arry objectAtIndex:i] copy] autorelease];
		obj.mandatory = NO;
		obj.key = [self proposedPropertyKey:obj.key];
		[objcts addObject:obj];
		i = [indexSet indexGreaterThanIndex:i];
	}
	[keyController insertObjects:objcts atArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(index, [objcts count])]];
}

-(IBAction)copyProp:(id)sender
{
	if (![self selectedObject]) return;
	NSIndexSet * indexSet = [keyController selectionIndexes];
	if ([indexSet firstIndex]==NSNotFound) return;
	NSPasteboard * cb = [NSPasteboard generalPasteboard];
	NSArray * items = [[keyController arrangedObjects] objectsAtIndexes:indexSet];
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
		for (PHObjectProperty * obj in items)
		{
			obj.mandatory = NO;
			obj.key = [self proposedPropertyKey:obj.key];
		}
		
		NSIndexSet * indexSet = [keyController selectionIndexes];
		NSUInteger index = [indexSet lastIndex];
		if (index==NSNotFound)
			index = [[keyController content] count];
		else
			index++;
		[keyController insertObjects:items atArrangedObjectIndexes:[NSIndexSet indexSetWithIndexesInRange:NSMakeRange(index, [items count])]];
	}
}

-(BOOL)validateMenuItemProp:(NSMenuItem*)sender
{
	if ([[sender title] isEqual:@"Delete"]||
		[[sender title] isEqual:@"Duplicate"]||
		[[sender title] isEqual:@"Copy"])
	{
		NSIndexSet * indexSet = [keyController selectionIndexes];
		NSUInteger index = [indexSet lastIndex];
		if (index==NSNotFound)
			return NO;
	}
	return YES;
}

@end
