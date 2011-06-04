//
//  ItemInfoTable.m
//  Level Designer
//
//  Created by Marius Petcu on 4/8/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "ItemInfoTable.h"
#import "ObjectController.h"
#import "PHObjectProperty.h"
#import "PHObject.h"

@implementation ItemInfoTable
@synthesize undoManager;

-(IBAction)new:(id)sender
{
	[(ObjectController*)self.delegate newProp:sender];
}

-(IBAction)delete:(id)sender
{
	[(ObjectController*)self.delegate deleteProp:sender];
}

-(IBAction)duplicate:(id)sender
{
	[(ObjectController*)self.delegate duplicateProp:sender];
}

-(IBAction)copy:(id)sender
{
	[(ObjectController*)self.delegate copyProp:sender];
}

-(IBAction)paste:(id)sender
{
	[(ObjectController*)self.delegate pasteProp:sender];
}

-(BOOL)validateMenuItem:(NSMenuItem*)sender;
{
	return [(ObjectController*)self.delegate validateMenuItemProp:sender];
}

-(void)setKeyController:(id)kc
{
	keyController = kc;
}

-(void)_setType:(int)ty andValue:(id)value forProp:(PHObjectProperty*)prp
{
    [[undoManager prepareWithInvocationTarget:self] _setType:prp.type andValue:prp.value forProp:prp];
    [prp setType:ty];
    [prp setValue:value];
    [[prp parentObject] modified];
}

-(void)setType:(int)ty andValue:(id)value forProp:(PHObjectProperty*)prp
{
    BOOL wasCollection = (prp.type == kPHObjectPropertyTree || prp.type == kPHObjectPropertyArray);
    BOOL isCollection = (ty == kPHObjectPropertyTree || ty == kPHObjectPropertyArray);
    [undoManager beginUndoGrouping];
    [self _setType:ty andValue:value forProp:prp];
    if (wasCollection && isCollection)
        [prp fixArrayKeysUndoable:undoManager];
    [undoManager endUndoGrouping];
}

-(void)convertToString
{
    [self setType:kPHObjectPropertyString andValue:prop.value forProp:prop];
}

-(void)convertToBool
{
    [self setType:kPHObjectPropertyBool andValue:prop.value forProp:prop];
}

-(void)convertToNumber
{
    [self setType:kPHObjectPropertyNumber andValue:prop.value forProp:prop];
}

-(void)convertToTree
{
    [self setType:kPHObjectPropertyTree andValue:prop.value forProp:prop];
}

-(void)convertToArray
{
    [self setType:kPHObjectPropertyArray andValue:prop.value forProp:prop];
}

-(NSMenu*)menuForEvent:(NSEvent *)event
{
	NSPoint pt=[self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:pt];
	if (row!=-1)
	{
        NSTreeNode * item = [self itemAtRow:row];
        PHObjectProperty * obj = (PHObjectProperty*)[item representedObject];
		if (obj.mandatory)
			return nil;
		NSMenu * menu = [[[NSMenu alloc] initWithTitle:@"Value Type"] autorelease];
		NSMenuItem * menuitem;
		
        prop = obj;
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"String"];
		[menuitem setTarget:self];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyString];
		[menuitem setAction:@selector(convertToString)];
		[menu addItem:menuitem];
		
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Number"];
		[menuitem setTarget:self];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyNumber];
		[menuitem setAction:@selector(convertToNumber)];
		[menu addItem:menuitem];
		
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Bool"];
		[menuitem setTarget:self];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyBool];
		[menuitem setAction:@selector(convertToBool)];
		[menu addItem:menuitem];
        
        menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Tree"];
		[menuitem setTarget:self];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyTree];
		[menuitem setAction:@selector(convertToTree)];
		[menu addItem:menuitem];

        menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Array"];
		[menuitem setTarget:self];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyArray];
		[menuitem setAction:@selector(convertToArray)];
		[menu addItem:menuitem];

		[self setMenu:menu];
		menu = [super menuForEvent:event];
		[self setMenu:nil];
		return menu;
	}
	return nil;
}


@end
