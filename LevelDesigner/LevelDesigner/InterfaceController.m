//
//  InterfaceController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "InterfaceController.h"
#import "ObjectController.h"
#import "PropertyController.h"
#import "PLEntity.h"
#import "PLObject.h"
#import "PLJoint.h"

@implementation InterfaceController

- (id)init
{
    self = [super initWithTables:ObjectController_numberOfArrays 
               andPasteboardType:PLObjectPBoardType
       andLocationPasteboardType:PLObjectLocationPBoardType
        andPointerPasteboardType:PLObjectPointerPBoardType];
    return self;
}

-(PLTableView*)objectView
{
    return tables[0];
}

-(PLTableView*)jointView
{
    return tables[1];
}

-(void)setObjectView:(PLTableView *)objectView
{
    [objectView retain];
    [tables[0] release];
    tables[0] = objectView;
}

-(void)setJointView:(PLTableView *)jointView
{
    [jointView retain];
    [tables[1] release];
    tables[1] = jointView;
}

-(void)prepareDetailsView
{
    PLEntity * newEntity = [model selectedEntity];
    BOOL isObject = [newEntity isKindOfClass:[PLObject class]];
    BOOL isJoint = [newEntity isKindOfClass:[PLJoint class]];
    currentEntity = newEntity;
    [propertyController setModel:isObject?[(PLObject*)newEntity rootProperty]:nil];
    [subentitiesController setModel:isObject?[(PLObject*)newEntity subentityModel]:nil];
    [jointController setModel:isJoint?newEntity:nil];
    
    [jointDetailView removeFromSuperview];
    [objectDetailView removeFromSuperview];
    NSView * view = nil;
    if (isObject)
        view = objectDetailView;
    else
        if (isJoint)
            view = jointDetailView;
    if (view)
    {
        [detailView addSubview:view];
        [view setFrame:[detailView bounds]];
    }
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [self prepareDetailsView];
    [super selectionForArrayChanged:array];
}

@end
