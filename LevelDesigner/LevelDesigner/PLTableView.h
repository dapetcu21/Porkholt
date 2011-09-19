//
//  PLTableView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface PLTableView : NSTableView

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(IBAction)duplicate:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender;

@end
