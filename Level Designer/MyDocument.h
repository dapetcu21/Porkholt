//
//  MyDocument.h
//  Level Designer
//
//  Created by Marius Petcu on 4/4/11.
//  Copyright 2011 Home. All rights reserved.
//


#import <Cocoa/Cocoa.h>

@class ObjectController;
@class FileBrowserController;

@interface MyDocument : NSDocument
{
	IBOutlet ObjectController * objectController;
	IBOutlet FileBrowserController * browserController;
}

@property(nonatomic,retain) NSFileWrapper * bundle;
@property(nonatomic,retain) NSFileWrapper * initScript;

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)duplicate:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)sender;

@end
