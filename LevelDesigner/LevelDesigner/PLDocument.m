//
//  PLDocument.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLDocument.h"
#import "ObjectController.h"
#import "FileBrowserController.h"

@implementation PLDocument
@synthesize objectController;
@synthesize browserController;

- (id)init
{
    self = [super init];
    if (self) {
    }
    return self;
}

-(void)dealloc
{
    [objectController release];
    [browserController release];
    [super dealloc];
}

- (NSString *)windowNibName
{
   return @"PLDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    [super windowControllerDidLoadNib:aController];
	[browserController setRootURL:[self fileURL]];
    [objectController setUndoManager:self.undoManager];
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
	[browserController performSelector:@selector(reload:) withObject:self afterDelay:0];
	return YES;
}


- (BOOL)readFromURL:(NSURL*)url ofType:(NSString *)typeName error:(NSError **)outError
{
	[browserController setRootURL:url];
	[objectController loadFromFile:url];
    return YES;
}

+ (BOOL)autosavesInPlace
{
    return NO;
}

@end
