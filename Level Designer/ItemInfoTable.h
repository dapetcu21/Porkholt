//
//  ItemInfoTable.h
//  Level Designer
//
//  Created by Marius Petcu on 4/8/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PHObjectProperty;

@interface ItemInfoTable : NSOutlineView {
	NSArrayController * keyController;
    NSUndoManager * undoManager;
    PHObjectProperty * prop;
}


-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)duplicate:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)sender;

-(void)setKeyController:(id)kc;
@property(nonatomic,assign) NSUndoManager * undoManager;
@end
