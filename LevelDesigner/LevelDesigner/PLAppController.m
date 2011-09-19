//
//  PHAppController.m
//  Level Designer
//
//  Created by Marius Petcu on 4/5/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PLAppController.h"


@implementation PLAppController


-(BOOL)isResourcePathValid:(NSString*)path
{
    if (path == nil) return NO;
    BOOL dir;
    return ([[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&dir] && dir);
}

-(void)applicationDidFinishLaunching:(NSNotification *)notification
{
    [NSBundle loadNibNamed:@"ResourcePathAccesory" owner:self];
    resourcePath = [[[NSUserDefaults standardUserDefaults] stringForKey:@"resourcePath"] retain];
    if (![self isResourcePathValid:resourcePath])
        [self resourcePathDialog:self];
}

-(void)dealloc
{
    [resourcePath release];
    [super dealloc];
}

-(void)setResourcePath:(NSString *)path
{
    [path retain];
    [resourcePath release];
    resourcePath = path;
    NSUserDefaults * def = [NSUserDefaults standardUserDefaults];
    [def setObject:path forKey:@"resourcePath"];
    [def synchronize];
}

-(NSString*)resourcePath{
    return resourcePath;
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender
{
    if ([[[NSDocumentController sharedDocumentController] documents] count]==0)
        [[NSDocumentController sharedDocumentController] openDocument:self];
	return NO;
}

-(IBAction)resourcePathDialog:(id)sender
{
    BOOL ok = false;
    NSAutoreleasePool * pool = nil;
    while (!ok)
    {
        [pool drain];
        pool = [[NSAutoreleasePool alloc] init];
        
        NSOpenPanel * panel = [[[NSOpenPanel alloc] init] autorelease];
        [panel setAllowsMultipleSelection:NO];
        [panel setCanChooseFiles:NO];
        [panel setCanChooseDirectories:YES];
        [panel setAccessoryView:resourcePathAccesoryView];
        
        NSInteger result = [panel runModal];
        
        ok = (result == NSOKButton);
        if (!ok) 
        {
            if ([self isResourcePathValid:resourcePath])
            {
                ok = YES;
                continue;
            }
            else
                exit(0);
        }
        
        NSString * fileName = [[[panel URLs] objectAtIndex:0] path]; 
        if ([self isResourcePathValid:fileName])
            [self setResourcePath:fileName];
        else 
        {
            ok = false;
            continue;
        }
    }
    [pool drain];
}

@end			