//
//  PHObject.h
//  Level Designer
//
//  Created by Marius Petcu on 4/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PHObjectProperty;
@class ObjectView;
@class ObjectController;

@interface PHObject : NSObject<NSCopying,NSCoding> {
	NSString * className;
	NSMutableArray * properties;
	PHObjectProperty * classProperty, *posXProperty, *posYProperty, *rotationProperty, *posProperty, *imagesProperty, * physicsProperty, * fixturesProperty;
	BOOL readOnly;
	BOOL selected,initialSelected;
	ObjectView * view;
	ObjectController * controller;
}
@property(nonatomic,assign) NSString * className; 
@property(nonatomic,assign) NSPoint position;
@property(nonatomic,assign) double rotation;

@property(nonatomic,assign) PHObjectProperty * classProperty;
@property(nonatomic,assign) PHObjectProperty * posXProperty;
@property(nonatomic,assign) PHObjectProperty * posYProperty;
@property(nonatomic,assign) PHObjectProperty * posProperty;
@property(nonatomic,assign) PHObjectProperty * rotationProperty;
@property(nonatomic,assign) PHObjectProperty * imagesProperty;
@property(nonatomic,assign) PHObjectProperty * physicsProperty;
@property(nonatomic,assign) PHObjectProperty * fixturesProperty;
@property(nonatomic,assign) BOOL readOnly;
@property(nonatomic,assign) BOOL editable;

-(void)move:(NSPoint)mv;
-(void)undoableMove:(NSPoint)mv;
-(void)undoableSetPosition:(NSPoint)pos;
-(void)positionChanged;
-(void)rebuildView;

-(PHObjectProperty*)propertyForKey:(NSString*)key;
-(void)searchForVitalProperties;

-(void)setProperties:(NSArray*)obj;
-(NSMutableArray*)properties;

-(id)initFromLua:(lua_State*)L;
-(void)saveToFile:(NSMutableString*)file;

-(void)modified;

-(void)updateSelected:(BOOL)val;

@property(nonatomic,assign) ObjectView * view;
@property(nonatomic,assign,getter=isSelected) BOOL selected;
@property(nonatomic,assign) ObjectController * controller;
@property(nonatomic,assign) BOOL initialSelected;
@end
