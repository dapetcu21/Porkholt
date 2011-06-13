//
//  MyDocument.m
//  Level Designer
//
//  Created by Marius Petcu on 4/4/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "MyDocument.h"
#import "ObjectController.h"
#import "WorldController.h"
#import "FileBrowserController.h"

@implementation MyDocument
@synthesize bundle;
@synthesize initScript;
@synthesize objectController;
@synthesize browserController;
@synthesize worldController;
- (id)init
{
    self = [super init];
    if (self) {
    }
    return self;
}

-(void)dealloc
{
	[bundle release];
	[initScript release];
    [objectController release];
    [browserController release];
    [worldController release];
	[super dealloc];
}

- (NSString *)windowNibName
{
    return @"MyDocument";
}

-(void)reloadBrowser
{
	[browserController setRootURL:[self fileURL]];
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
	[browserController setRootURL:[self fileURL]];
	[objectController loadFromFile:[self fileURL]];
}

- (BOOL)writeToURL:(NSURL *)absoluteURL ofType:(NSString *)typeName forSaveOperation:(NSSaveOperationType)saveOperation originalContentsURL:(NSURL *)absoluteOriginalContentsURL error:(NSError **)outError
{
	NSFileManager * man = [NSFileManager defaultManager];
	NSError * err = nil;
	if (!absoluteOriginalContentsURL)
		absoluteOriginalContentsURL = [[[NSBundle mainBundle] resourceURL] URLByAppendingPathComponent:@"prototype.phlevel"];

	[man copyItemAtURL:absoluteOriginalContentsURL toURL:absoluteURL error:&err];
	if (err&&outError)
	{
		*outError = [NSError errorWithDomain:@"PorkholtDomain" code:-2 userInfo:
					 [NSDictionary dictionaryWithObject:[@"Can't create bundle: " stringByAppendingString:[[err userInfo] valueForKey:NSLocalizedDescriptionKey]] forKey:NSLocalizedDescriptionKey]];
		return NO;
	}

	NSMutableString * generatedFile = [[[NSMutableString alloc] init] autorelease];
	[objectController saveToFile:generatedFile];
	if (![man createFileAtPath:[[absoluteURL URLByAppendingPathComponent:@"lvl_designer.lua"] path] contents:[generatedFile dataUsingEncoding:NSUTF8StringEncoding] attributes:nil]&&outError)
	{
		*outError = [NSError errorWithDomain:@"PorkholtDomain" code:-1 userInfo:
					  [NSDictionary dictionaryWithObject:@"Can't write lvl_designer.lua" forKey:NSLocalizedDescriptionKey]];
		return NO;
	}
	[self performSelector:@selector(reloadBrowser) withObject:nil afterDelay:0];
	return YES;
}

- (BOOL)readFromURL:(NSURL*)url ofType:(NSString *)typeName error:(NSError **)outError
{
	
	[browserController setRootURL:url];
	[objectController loadFromFile:url];
    return YES;
}

-(IBAction)new:(id)sender
{
	[objectController new:sender];
}

-(IBAction)delete:(id)sender
{
	[objectController delete:sender];
}

-(IBAction)duplicate:(id)sender
{
	[objectController duplicate:sender];
}

-(IBAction)copy:(id)sender
{
	[objectController copy:sender];
}

-(IBAction)paste:(id)sender
{
	[objectController paste:sender];
}

-(IBAction)scrollToOrigin:(id)sender
{
    [worldController.worldView scrollToOrigin:sender];
}

-(IBAction)resetAspectRatio:(id)sender
{
    [worldController.worldView resetAspectRatio:sender];
}

-(BOOL)validateMenuItem:(NSMenuItem*)sender
{
	return [objectController validateMenuItem:sender];
}

-(NSURL*)resourceURLNamed:(NSString*)str
{
    if (!str || [str length]==0) return nil;
    if ([str UTF8String][0]=='/')
    {
        str = [str substringFromIndex:1];
        return [[[[[NSBundle mainBundle] resourceURL] URLByAppendingPathComponent:@"rsrc"] URLByAppendingPathComponent:@"img"] URLByAppendingPathComponent:str];
    } else {
        return [[self fileURL] URLByAppendingPathComponent:str];
    }
}

-(IBAction)toggleMatching:(id)sender
{
    [objectController toggleMatching:sender];
}

-(IBAction)sendToBack:(id)sender
{
    [objectController sendToBack:sender];
}

-(IBAction)bringToFront:(id)sender
{
    [objectController bringToFront:sender];
}

@end
