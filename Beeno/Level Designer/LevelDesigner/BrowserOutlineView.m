//
//  BrowserOutlineView.m
//  Level Designer
//
//  Created by Marius Petcu on 4/6/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "BrowserOutlineView.h"
#import "FileSystemNode.h"
#import "FileBrowserController.h"

@implementation BrowserOutlineView

-(void)awakeFromNib
{
	[self setDoubleAction:@selector(doubleClick)];
	NSMenu * menu = [[[NSMenu alloc] initWithTitle:@"Files"] autorelease];
	NSMenuItem * menuitem;
	
	menuitem = [[[NSMenuItem alloc] init] autorelease];
	[menuitem setTitle:@"Open"];
	[menuitem setTarget:self];
	[menuitem setAction:@selector(openInFinder)];
	[menu addItem:menuitem];
	
	menuitem = [[[NSMenuItem alloc] init] autorelease];
	[menuitem setTitle:@"Reveal in Finder"];
	[menuitem setTarget:self];
	[menuitem setAction:@selector(revealInFinder)];
	[menu addItem:menuitem];
	
	menuitem = [[[NSMenuItem alloc] init] autorelease];
	[menuitem setTitle:@"Reload"];
	[menuitem setTarget:self.delegate];
	[menuitem setAction:@selector(reload:)];
	[menu addItem:menuitem];
	
	[self setMenu:menu];
}

-(NSMenu*)menuForEvent:(NSEvent *)event
{
	NSPoint pt=[self convertPoint:[event locationInWindow] fromView:nil];
	if ([self rowAtPoint:pt]!=-1)
		return [super menuForEvent:event];
	NSMenu * menu = [[[NSMenu alloc] initWithTitle:@"Files"] autorelease];
	NSMenuItem * menuitem = [[[NSMenuItem alloc] init] autorelease];
	[menuitem setTitle:@"Reload"];
	[menuitem setTarget:self.delegate];
	[menuitem setAction:@selector(reload:)];
	[menu addItem:menuitem];
	return menu;
}

-(void)openInFinder
{
	NSInteger cr = [self clickedRow];
	if (cr==-1) return;
	FileSystemNode * node = [[[self itemAtRow:cr] representedObject] representedObject];
	if ([node.url isEqual:((FileBrowserController*)self.delegate).rootURL])
	{
		//[[NSWorkspace sharedWorkspace] openFile:[node.url path] withApplication:@"Finder"];
		[[NSWorkspace sharedWorkspace] selectFile:[[node.url URLByAppendingPathComponent:@"init.lua"] path] inFileViewerRootedAtPath:nil]; //THIS IS SICK
	}
	else
		[[NSWorkspace sharedWorkspace] openURL:node.url];
}

-(void)revealInFinder
{
	NSInteger cr = [self clickedRow];
	if (cr==-1) return;
	FileSystemNode * node = [[[self itemAtRow:cr] representedObject] representedObject];
	[[NSWorkspace sharedWorkspace] selectFile:[node.url path] inFileViewerRootedAtPath:nil];
}

-(void)doubleClick
{
	if ([self clickedRow]!=-1)
		[self openInFinder];
}

-(void)keyDown:(NSEvent *)event
{
    if ([[event characters] isEqualToString:@"r"] && ([event modifierFlags] & NSCommandKeyMask))
        [(FileBrowserController*)[self delegate] reload:self];
    else
        [super keyDown:event];
}

@end
