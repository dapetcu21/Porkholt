//
//  FileBrowserController.h
//  Level Designer
//
//  Created by Marius Petcu on 4/6/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface FileBrowserController : NSObject<NSOutlineViewDelegate> {
	IBOutlet NSTreeController * treeController;
	IBOutlet NSOutlineView * outlineView;
	NSMutableArray * contents;
    IBOutlet NSButton * reloadButton;
	
	NSCell * emptyCell;
	NSButtonCell * checkboxCell;
	
	NSURL * rootURL;
}

@property(nonatomic,retain) NSURL * rootURL;

- (void)setContents:(NSArray *)newContents;
- (NSMutableArray *)contents;

-(IBAction)reload:(id)sender;
-(void)clear;
-(void)setRootURL:(NSURL *)path;

-(NSURL*)selectedFile;

@end
