//
//  PHObjectProperty.h
//  Level Designer
//
//  Created by Marius Petcu on 4/7/11.
//  Copyright 2011 Home. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class PHObject;

enum 
{
	kPHObjectPropertyString = 0,
	kPHObjectPropertyNumber,
	kPHObjectPropertyBool,
    kPHObjectPropertyTree,
    kPHObjectPropertyArray
};

@interface PHObjectProperty : NSTreeNode<NSCoding,NSCopying> {
	id value;
	NSString * key;
	PHObject * parentObject;
	int type;
	BOOL mandatory;
}

-(PHObjectProperty*)propertyForKey:(NSString*)key;
-(PHObjectProperty*)propertyAtIndex:(NSUInteger)index;

-(id)initWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;
+(PHObjectProperty*)propertyWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;
-(id)initMandatoryWithValue:(id)v ofType:(int)ty  forKey:(NSString*)k;
+(PHObjectProperty*)mandatoryPropertyWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;

-(void)convertToString;
-(void)convertToNumber;
-(void)convertToBool;
-(void)convertToTree;
-(void)convertToArray;

-(void)fixArrayKeysUndoable:(NSUndoManager*)man;

@property(nonatomic,retain) id value;
@property(nonatomic,retain) NSString * key;
@property(nonatomic,getter=isMandatory) BOOL mandatory;
@property(nonatomic,assign) int type;

-(void)setUndoable:(NSUndoManager*)man key:(NSString*)key;
-(void)setUndoable:(NSUndoManager*)man doubleValue:(double)val;
-(void)setUndoable:(NSUndoManager*)man boolValue:(BOOL)val;
-(void)setUndoable:(NSUndoManager*)man stringValue:(NSString*)val;
-(void)setUndoable:(NSUndoManager*)man value:(id)val;

-(void)setUndoable:(NSUndoManager*)man type:(int)type andValue:(id)val;

-(void)convertToString;
-(void)convertToBool;
-(void)convertToNumber;
-(void)convertToTree;

-(void)loadFromLua:(lua_State*)L;

@property(nonatomic,assign) PHObject * parentObject;

@property(nonatomic,assign) double doubleValue;
@property(nonatomic,assign) int intValue;
@property(nonatomic,assign) BOOL boolValue;
@property(nonatomic,retain) NSString * stringValue;

@end
