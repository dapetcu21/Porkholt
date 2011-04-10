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

@implementation ItemInfoTable

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

-(NSMenu*)menuForEvent:(NSEvent *)event
{
	NSPoint pt=[self convertPoint:[event locationInWindow] fromView:nil];
	int row = [self rowAtPoint:pt];
	if (row!=-1)
	{
		PHObjectProperty * obj = [[keyController arrangedObjects] objectAtIndex:row];
		if (obj.mandatory)
			return nil;
		NSMenu * menu = [[[NSMenu alloc] initWithTitle:@"Value Type"] autorelease];
		NSMenuItem * menuitem;
		
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"String"];
		[menuitem setTarget:obj];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyString];
		[menuitem setAction:@selector(convertToString)];
		[menu addItem:menuitem];
		
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Number"];
		[menuitem setTarget:obj];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyNumber];
		[menuitem setAction:@selector(convertToNumber)];
		[menu addItem:menuitem];
		
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Bool"];
		[menuitem setTarget:obj];
		[menuitem setEnabled:YES];
		[menuitem setState:obj.type==kPHObjectPropertyBool];
		[menuitem setAction:@selector(convertToBool)];
		[menu addItem:menuitem];
		
		[self setMenu:menu];
		menu = [super menuForEvent:event];
		[self setMenu:nil];
		return menu;
	}
	return nil;
}


@end
