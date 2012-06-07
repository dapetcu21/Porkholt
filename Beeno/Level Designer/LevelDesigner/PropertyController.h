/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>

@class PLProperty;
@interface PropertyController : NSObject<NSOutlineViewDelegate,NSOutlineViewDataSource>
{
    IBOutlet NSOutlineView * outlineView;
    PLProperty * model;
    BOOL propertyChangedFromWithin;
    BOOL readOnlyModel;
    IBOutlet NSCell * boolCell;
}

@property(nonatomic,retain) PLProperty * model;

-(void)new:(id)sender;
-(void)delete:(id)sender;
-(void)copy:(id)sender;
-(void)paste:(id)sender;
-(void)duplicate:(id)sender;
-(BOOL)validateMenuItem:(NSMenuItem*)menuItem sentFrom:(id)sender;

-(void)propertyChanged:(PLProperty*)p;
-(NSArray*)filteredPropertiesWithoutChildren:(NSArray*)props;
-(NSArray*)filteredPropertiesWithoutMandatory:(NSArray*)props;

@end
