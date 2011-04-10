//
//  FileBrowserController.m
//  Level Designer
//
//  Created by Marius Petcu on 4/6/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "FileBrowserController.h"
#import "FileSystemNode.h"
#import "ImageAndTextCell.h"
#import "BrowserTreeNode.h"

#define COLUMNID_FILES @"Files"
#define COLUMNID_AA @"AntiAliasing"

@implementation FileBrowserController
@synthesize rootURL;

- (id)init
{
	if (self = [super init])
	{
		contents = [[NSMutableArray alloc] init];
	}
	return self;
}

- (void)dealloc
{
	[contents release];
	[rootURL release];
	[emptyCell release];
	[checkboxCell release];
	[super dealloc];
}

-(void)awakeFromNib
{
	NSTableColumn *tableColumn = [outlineView tableColumnWithIdentifier:COLUMNID_FILES];
	ImageAndTextCell *imageAndTextCell = [[[ImageAndTextCell alloc] init] autorelease];
	[tableColumn setDataCell:imageAndTextCell];
	emptyCell = [[NSCell alloc] init];
	checkboxCell = [[NSButtonCell alloc] init];
	[checkboxCell setButtonType:NSSwitchButton];
	[checkboxCell setTitle:nil];
}

- (void)setContents:(NSArray*)newContents
{
	if (contents != newContents)
	{
		[contents release];
		contents = [[NSMutableArray alloc] initWithArray:newContents];
	}
}

- (NSMutableArray *)contents
{
	return contents;
}

-(void)clear
{
	[self setContents:[NSArray array]];
	[treeController setContent:contents];
}

-(void)addURL:(NSURL*)url atIndexPath:(NSIndexPath*)indexPath
{
	NSString * path = [url path];
	NSString * extension = [path pathExtension];
	FileSystemNode * fs = [[[FileSystemNode alloc] init] autorelease];
	fs.title = [url lastPathComponent];
	fs.url = url;
	fs.icon = [[NSWorkspace sharedWorkspace] iconForFile:path];
	fs.antiAliasing = FALSE;
	NSFileManager * fman = [NSFileManager defaultManager];
	if ([extension isEqualToString:@"png"])
		fs.antiAliasing = ![fman fileExistsAtPath:[path stringByAppendingPathExtension:@"noaa"]];
	NSTreeNode * node = [[[BrowserTreeNode alloc] initWithRepresentedObject:fs] autorelease];
	[treeController insertObject:node atArrangedObjectIndexPath:indexPath];
	BOOL isDir;
	[fman fileExistsAtPath:path isDirectory:&isDir];
	if (isDir)
	{
		NSArray * dirs = [fman contentsOfDirectoryAtPath:path error:NULL];
		for (NSString * fpath in dirs)
		{
			NSString * fname = [fpath lastPathComponent];
			if ([fname isEqualToString:@".svn"]) continue;
			NSString * ext = [fname pathExtension];
			if ([ext isEqualToString:@"noaa"]) continue;
			[self addURL:[url URLByAppendingPathComponent:fname] 
			 atIndexPath:[indexPath indexPathByAddingIndex:[[node childNodes] count]]];
		}
	}
}

-(void)reload:(id)sender
{
	[self clear];
	if (!rootURL) return;
	[self addURL:rootURL atIndexPath:[NSIndexPath indexPathWithIndex:[contents count]]];
	[treeController setSelectionIndexPaths:[NSArray array]];
}

-(void)setRootURL:(NSURL *)path;
{
	[path retain];
	[rootURL release];
	rootURL = path;
	[self reload:self];
}

- (NSCell *)outlineView:(NSOutlineView *)outlineView dataCellForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
	if ([[tableColumn identifier] isEqualToString:COLUMNID_FILES])
		return  [tableColumn dataCell];
	if ([[tableColumn identifier] isEqualToString:COLUMNID_AA])
	{
		FileSystemNode * it = (FileSystemNode*)[[item representedObject] representedObject];
		if ([[[it url] pathExtension] isEqualToString:@"png"])
			return checkboxCell;
		else
			return emptyCell;
	}
	return nil;
}

- (void)outlineView:(NSOutlineView *)olv willDisplayCell:(NSCell*)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item
{	 
	if ([[tableColumn identifier] isEqualToString:COLUMNID_FILES])
	{
		item = [[item representedObject] representedObject];
		[(ImageAndTextCell*)cell setTitle:[item title]];
		[(ImageAndTextCell*)cell setImage:[item icon]];
	}
	if ([[tableColumn identifier] isEqualToString:COLUMNID_AA])
	{
		if ([cell isKindOfClass:[NSButtonCell class]])
		{
			item = [[item representedObject] representedObject];
			[(NSButtonCell*)cell setTarget:item];
			[(NSButtonCell*)cell setAction:@selector(toggleAntialiasing:)];
			[(NSButtonCell*)cell setState:[item antiAliasing]];
		}
	}
}


@end
