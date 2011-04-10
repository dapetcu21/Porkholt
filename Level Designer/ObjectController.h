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

@interface ObjectController : NSObject<NSTableViewDelegate> {
	NSMutableArray * objects;
	
	IBOutlet NSArrayController * arrayController;
	IBOutlet NSArrayController * keyController;
	IBOutlet NSTableView * objectBrowser; 
	IBOutlet ItemInfoTable * itemInfo; 
	NSIndexSet * draggedObjects;
	NSIndexSet * draggedKeys;
	
	IBOutlet NSButtonCell * boolCell;
	IBOutlet NSNumberFormatter * numberFormatter;
	
	int readOnlyCells;
}

-(void)setObjects:(NSArray*)obj;
-(NSMutableArray*)objects;
-(PHObject*)selectedObject;

-(NSString*)proposedPropertyKey:(NSString*)name;

-(void)lua:(lua_State*)L setPath:(NSString*)path;
-(void)loadFromFile:(NSURL*)url;
-(void)saveToFile:(NSMutableString*)file;

-(IBAction)new:(id)sender;
-(IBAction)delete:(id)sender;
-(IBAction)duplicate:(id)sender;
-(IBAction)copy:(id)sender;
-(IBAction)paste:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)sender;

-(IBAction)newProp:(id)sender;
-(IBAction)deleteProp:(id)sender;
-(IBAction)duplicateProp:(id)sender;
-(IBAction)copyProp:(id)sender;
-(IBAction)pasteProp:(id)sender;
-(BOOL)validateMenuItemProp:(NSMenuItem*)sender;


@end
