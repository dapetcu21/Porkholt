//
//  ItemInfoTable.h
//  Level Designer
//
//  Created by Marius Petcu on 4/8/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface ItemInfoTable : NSTableView {
	NSArrayController * keyController;
}

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)duplicate:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)sender;
-(void)setKeyController:(id)kc;
@end
