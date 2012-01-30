//
//  PLProperty.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/23/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"
@class PropertyController;

enum PLPropertyTypes
{
    PLPropertyNil = 0,
    PLPropertyNumber,
    PLPropertyBoolean,
    PLPropertyString,
    PLPropertyArray,
    PLPropertyDictionary,
    PLPropertyPoint,
    PLPropertyRect,
    PLPropertyNumberOfTypes,
};

@interface PLProperty : PLEntity
{
    union {
        BOOL boolean;
        double number;
        NSMutableArray * array;
        struct
        {
            NSMutableDictionary * value;
            NSMutableArray * orderedKeys;
        } dictionary;
        NSPoint point;
        NSRect rect;
        NSString * string;
    } value;
    
    int type;
    NSString * name;
    NSUInteger position;
    PLProperty * parent;
    BOOL mandatory;
    PropertyController * controller;
    BOOL mark;
}

@property(nonatomic,retain) NSString * name;
@property(nonatomic,readonly) NSUInteger position;
@property(nonatomic,assign) BOOL mandatory;
@property(nonatomic,assign) PropertyController * controller;
@property(nonatomic,readonly) NSUndoManager * undoManager;
@property(nonatomic,readonly) PLProperty * parent;
@property(nonatomic,assign) BOOL mark;

@property(nonatomic,assign) int type;
@property(nonatomic,assign) BOOL booleanValue;
@property(nonatomic,assign) double numberValue;
@property(nonatomic,assign) NSString * stringValue;
@property(nonatomic,assign) NSArray * arrayValue;
@property(nonatomic,assign) NSDictionary * dictionaryValue;
@property(nonatomic,assign) NSPoint pointValue;
@property(nonatomic,assign) NSRect rectValue;
-(void)setRetainedArrayValue:(NSMutableArray*)a;
-(void)setRetainedDictionaryValue:(NSMutableDictionary *)dictionaryValue retainedOrder:(NSMutableArray*)order;
-(void)setDictionaryValue:(NSDictionary *)dictionaryValue order:(NSArray*)order;
@property(nonatomic,readonly) NSArray * dictionaryOrder;
-(void)setPointValueFromString:(NSString*)s;
-(void)setRectValueFromString:(NSString*)s;
-(void)setNil;

-(void)convertToString;
-(void)convertToNumber;
-(void)convertToBoolean;
-(void)convertToArray;
-(void)convertToDictionary;
-(void)convertToPoint;
-(void)convertToRect;

-(PLProperty*)propertyAtKeyPath:(NSString*)path;
-(PLProperty*)propertyWithKey:(NSString*)key;
-(PLProperty*)propertyAtIndex:(NSUInteger)index;
-(NSUInteger)childrenCount;

-(NSString*)proposedKeyForName:(NSString*)name;
-(NSString*)proposedKeyForName:(NSString*)name withAdditions:(NSArray*)add;
-(void)rebuildArrayNames;

-(void)renameChild:(PLProperty*)child to:(NSString*)newName;

-(NSIndexSet*)indexesForProperties:(NSArray*)props;
-(NSArray*)propertiesForIndexes:(NSIndexSet*)indexes;

-(void)insertProperties:(NSArray*)props atIndexes:(NSIndexSet*)indexes;
-(void)insertProperties:(NSArray*)props atIndex:(NSUInteger)index;
-(void)insertProperty:(PLProperty*)p atIndex:(NSUInteger)index;
-(void)removeProperties:(NSArray*)props;
-(void)removePropertiesAtIndexes:(NSIndexSet*)indexes;
-(void)removeProperty:(PLProperty*)p;
-(void)removeProperties:(NSArray*)props atIndexes:(NSIndexSet*)indexes; //use this if you know both the props and the indexes


-(NSMutableArray*)keysForArray:(NSArray*)arr;

-(BOOL)isCollection;

-(void)writeToFile:(NSMutableString*)file withIndexPath:(NSString*)indexPath;

@end
