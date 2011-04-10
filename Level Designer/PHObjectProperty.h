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
	kPHObjectPropertyBool
};

@interface PHObjectProperty : NSObject<NSCoding,NSCopying> {
	id value;
	NSString * key;
	PHObject * object;
	int type;
	BOOL mandatory;
}

-(id)initWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;
+(PHObjectProperty*)propertyWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;
-(id)initMandatoryWithValue:(id)v ofType:(int)ty  forKey:(NSString*)k;
+(PHObjectProperty*)mandatoryPropertyWithValue:(id)v ofType:(int)ty forKey:(NSString*)k;

-(void)convertToString;
-(void)convertToNumber;
-(void)convertToBool;

@property(nonatomic,retain) id value;
@property(nonatomic,retain) NSString * key;
@property(nonatomic,assign) PHObject * object;
@property(nonatomic,getter=isMandatory) BOOL mandatory;
@property(nonatomic,assign) int type;

@property(nonatomic,assign) double doubleValue;
@property(nonatomic,assign) int intValue;
@property(nonatomic,assign) BOOL boolValue;
@property(nonatomic,retain) NSString * stringValue;

@end
