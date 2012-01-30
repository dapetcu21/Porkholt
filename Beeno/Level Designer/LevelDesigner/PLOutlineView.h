//
//  PLOutlineView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface PLOutlineView : NSOutlineView

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(IBAction)duplicate:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)menuItem;

@end
