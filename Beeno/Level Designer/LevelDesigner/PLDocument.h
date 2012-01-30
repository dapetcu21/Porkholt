//
//  PLDocument.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class FileBrowserController;
@class ObjectController;

@interface PLDocument : NSDocument
{
    ObjectController * objectController;
    FileBrowserController * browserController;
    IBOutlet NSWindow * window;
}

@property(nonatomic,retain) IBOutlet ObjectController * objectController;
@property(nonatomic,retain) IBOutlet FileBrowserController * browserController;

-(IBAction)undo:(id)sender;
-(IBAction)redo:(id)sender;

@end
