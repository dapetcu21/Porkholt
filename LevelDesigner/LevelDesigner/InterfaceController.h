//
//  InterfaceController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class ObjectController;
@class PLTableView;
@interface InterfaceController : NSObject<NSTableViewDataSource,NSTableViewDelegate>
{
    IBOutlet PLTableView * objectView;
    IBOutlet PLTableView * jointView;
    IBOutlet ObjectController * model;
    IBOutlet NSTabView * tabView;
    BOOL arrayChangeFromWithin;
    BOOL selectionChangeFromWithin;
    int tabToMoveTo;
}

-(void)new:(id)sender;
-(void)delete:(id)sender;
-(void)copy:(id)sender;
-(void)paste:(id)sender;
-(void)duplicate:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender;

-(void)arrayChanged:(NSUInteger)array;
-(void)selectionForArrayChanged:(NSUInteger)array;

@end
