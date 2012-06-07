/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PLOutlineView.h"
#import "PropertyController.h"
#import "PLProperty.h"

@implementation PLOutlineView

-(void)new:(id)sender
{
    [(PropertyController*)self.delegate new:self];
}

-(void)delete:(id)sender
{
    [(PropertyController*)self.delegate delete:self];
}

-(void)copy:(id)sender
{
    [(PropertyController*)self.delegate copy:self];    
}

-(void)paste:(id)sender
{
    [(PropertyController*)self.delegate paste:self];    
}

-(void)duplicate:(id)sender
{
    [(PropertyController*)self.delegate duplicate:self];    
}

-(BOOL)validateMenuItem:(NSMenuItem*)menuItem
{
    return [(PropertyController*)self.delegate validateMenuItem:menuItem sentFrom:self];
}

-(NSMenu*)menuForEvent:(NSEvent *)event
{
	NSPoint pt=[self convertPoint:[event locationInWindow] fromView:nil];
	NSInteger row = [self rowAtPoint:pt];
	if (row!=-1)
	{
        PLProperty * prop = (PLProperty*)[self itemAtRow:row];
        if (!prop || prop.mandatory || prop.readOnly) 
            return nil;
		NSMenu * menu = [[[NSMenu alloc] initWithTitle:@"Value Type"] autorelease];
		NSMenuItem * menuitem;
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"String"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyString];
		[menuitem setAction:@selector(convertToString)];
		[menu addItem:menuitem];
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Number"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyNumber];
		[menuitem setAction:@selector(convertToNumber)];
		[menu addItem:menuitem];
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Boolean"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyBoolean];
		[menuitem setAction:@selector(convertToBoolean)];
		[menu addItem:menuitem];
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Array"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyArray];
		[menuitem setAction:@selector(convertToArray)];
		[menu addItem:menuitem];
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Dictionary"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyDictionary];
		[menuitem setAction:@selector(convertToDictionary)];
		[menu addItem:menuitem];
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Point"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyPoint];
		[menuitem setAction:@selector(convertToPoint)];
		[menu addItem:menuitem];
        
		menuitem = [[[NSMenuItem alloc] init] autorelease];
		[menuitem setTitle:@"Rectangle"];
		[menuitem setTarget:prop];
		[menuitem setEnabled:YES];
		[menuitem setState:prop.type==PLPropertyRect];
		[menuitem setAction:@selector(convertToRect)];
		[menu addItem:menuitem];
        
		return menu;
	}
	return nil;
}

@end
