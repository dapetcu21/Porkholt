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
@class WorldController;

@interface MyDocument : NSDocument
{
    ObjectController * objectController;
    WorldController * worldController;
    FileBrowserController * browserController;
}

@property(nonatomic,retain) NSFileWrapper * bundle;
@property(nonatomic,retain) NSFileWrapper * initScript;
@property(nonatomic,retain) IBOutlet ObjectController * objectController;
@property(nonatomic,retain) IBOutlet WorldController * worldController;
@property(nonatomic,retain) IBOutlet FileBrowserController * browserController;

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)duplicate:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)sender;
-(IBAction)scrollToOrigin:(id)sender;
-(IBAction)resetAspectRatio:(id)sender;

-(NSURL*)resourceURLNamed:(NSString*)str;

-(IBAction)toggleMatching:(id)sender;
-(IBAction)commitMatch:(id)sender;
-(IBAction)sendToBack:(id)sender;
-(IBAction)bringToFront:(id)sender;

@end
