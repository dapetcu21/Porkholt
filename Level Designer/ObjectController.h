//
//  ObjectController.h
//  Level Designer
//
//  Created by Marius Petcu on 4/5/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#define kObjectPBoardType @"PHObjectPBoardType"
#define kPropertyPBoardType @"PHPropertyPBoardType"


@class PHObject;
@class ItemInfoTable;
@class WorldController;
@class MyDocument;
@class PHObjectProperty;

@interface ObjectController : NSObject<NSTableViewDelegate> {
	NSMutableArray * objects;
	
	IBOutlet NSArrayController * arrayController;
	IBOutlet NSTreeController * keyController;
	IBOutlet NSTableView * objectBrowser; 
	IBOutlet ItemInfoTable * itemInfo; 
	NSIndexSet * draggedObjects;
	NSArray * draggedKeys;
    BOOL draggingMandatory;
	IBOutlet WorldController * worldController;
	IBOutlet MyDocument * document;
	
	IBOutlet NSButtonCell * boolCell;
	IBOutlet NSNumberFormatter * numberFormatter;
	
	int readOnlyCells;
	
	BOOL ignoreChange;
}

-(void)setObjects:(NSArray*)obj;
-(NSMutableArray*)objects;

-(PHObject*)selectedObject;
-(void)selectObject:(PHObject*)obj;

-(NSArrayController*)arrayController;

-(NSArray*)siblingsForItem:(NSTreeNode*)prop;
-(NSArray*)siblingsForIndexPath:(NSIndexPath*)path;
-(NSString*)proposedPropertyKey:(NSString*)name forSiblings:(NSArray*)arr andProp:(PHObjectProperty*)prp;

-(void)lua:(lua_State*)L setPath:(NSString*)path;
-(void)loadFromFile:(NSURL*)url;
-(void)saveToFile:(NSMutableString*)file;

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)duplicate:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)sender;

-(void)addObjects:(NSArray*)arry atIndexSet:(NSIndexSet*)iSet;
-(void)deleteIndexSet:(NSIndexSet*)indexSet;
-(void)insertProperties:(NSArray*)props atIndexPaths:(NSArray*)paths forObject:(PHObject*)obj;
-(void)removePropertiesAtIndexPaths:(NSArray*)paths forObject:(PHObject*)obj;

-(IBAction)newProp:(id)sender;
-(IBAction)deleteProp:(id)sender;
-(IBAction)duplicateProp:(id)sender;
-(IBAction)copyProp:(id)sender;
-(IBAction)pasteProp:(id)sender;
-(BOOL)validateMenuItemProp:(NSMenuItem*)sender;

-(MyDocument*)document;
-(NSUndoManager*)undoManager;

-(void)objectChanged:(PHObject*)obj;
//@property(nonatomic,retain) NSArray * objects;
//@property(nonatomic,retain) NSArray * selection;

@end
