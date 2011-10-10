//
//  MultipleListController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class EntityController;
@class PLTableView;

@interface MultipleListController : NSObject<NSTableViewDataSource,NSTableViewDelegate>
{
    EntityController * model;
    IBOutlet NSTabView * tabView;
    BOOL arrayChangeFromWithin;
    BOOL selectionChangeFromWithin;
    int tabToMoveTo;
    PLTableView ** tables;
    
    NSUInteger numberOfTables;
    NSString * pasteboardType;
    NSString * locationPasteboardType;
    NSString * pointerPasteboardType;
}

@property(nonatomic,retain) IBOutlet EntityController * model;

-(id)           initWithTables:(NSUInteger)array 
             andPasteboardType:(NSString*)pbType
     andLocationPasteboardType:(NSString*)lpbType
      andPointerPasteboardType:(NSString*)ppbType;


-(void)new:(id)sender;
-(void)delete:(id)sender;
-(void)copy:(id)sender;
-(void)paste:(id)sender;
-(void)duplicate:(id)sender;
-(void)selectAll:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender;

-(void)arrayChanged:(NSUInteger)array;
-(void)selectionForArrayChanged:(NSUInteger)array;

@end
