//
//  PHAppController.m
//  Level Designer
//
//  Created by Marius Petcu on 4/5/11.
//  Copyright 2011 Home. All rights reserved.
//

#import "PHAppController.h"


@implementation PHAppController

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	if ([[[NSDocumentController sharedDocumentController] documents] count]==0)
		[[NSDocumentController sharedDocumentController] openDocument:self];
}

- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender
{
	return NO;
}

@end			