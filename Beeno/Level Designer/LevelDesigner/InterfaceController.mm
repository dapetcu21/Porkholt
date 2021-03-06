/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "InterfaceController.h"
#import "ObjectController.h"
#import "PropertyController.h"
#import "PLEntity.h"
#import "PLObject.h"
#import "PLJoint.h"
#import "PLImage.h"
#import "PLImageView.h"
#import "SubentityController.h"
#import "JointController.h"

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

-(PLTableView*)curveView
{
    return tables[2];
}

-(void)setCurveView:(PLTableView *)objectView
{
    [objectView retain];
    [tables[2] release];
    tables[2] = objectView;
}

-(void)prepareDetailsView
{
    PLEntity * newEntity = [model selectedEntity];
    BOOL isObject = [newEntity isKindOfClass:[PLObject class]];
    BOOL isJoint = [newEntity isKindOfClass:[PLJoint class]];
    currentEntity = newEntity;
    [propertyController setModel:isObject?[(PLObject*)newEntity rootProperty]:nil];
    [subentitiesController setModel:isObject?[(PLObject*)newEntity subentityModel]:nil];
    [jointController setModel:isJoint?(PLJoint*)newEntity:nil];
    
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

-(BOOL)clearSelectionWhenSelecting:(NSUInteger)i
{
    //return (i!=2);
    return YES;
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [self prepareDetailsView];
    [super selectionForArrayChanged:array];
}

-(IBAction)toggleShowMarkers:(id)sender
{
    [(ObjectController*)model setShowMarkers:!((ObjectController*)model).showMarkers];
}

-(IBAction)toggleShowImages:(id)sender
{
    [(ObjectController*)model setShowImages:!((ObjectController*)model).showImages];
}

-(IBAction)toggleShowFixtures:(id)sender
{
    [(ObjectController*)model setShowFixtures:!((ObjectController*)model).showFixtures];
}

-(IBAction)toggleShowJoints:(id)sender
{
    [(ObjectController*)model setShowJoints:!((ObjectController*)model).showJoints];
}

-(IBAction)toggleObjectMode:(id)sender
{
    ObjectController * oc = (ObjectController*)model;
    if (![oc isObjectModePossible] && !oc.objectMode)
    {
        NSBeep();
        return;
    }
    [oc setObjectMode:!oc.objectMode];
}

-(IBAction)resetAspectRatio:(id)sender
{
    ObjectController * oc = (ObjectController*)model;
    PLObject * obj = (PLObject*)[oc selectedEntity];
    if (!oc.objectMode || ![obj isKindOfClass:[PLObject class]]) 
    {
        NSBeep();
        return;
    }
    PLImage * image = (PLImage*)[[obj subentityModel] selectedEntity];
    if (![image isKindOfClass:[PLImage class]] || !image.actor)
    {
        NSBeep();
        return;
    }
    (image.actor)->resetAspectRatio();
}

-(void)match:(id)sender
{
    ObjectController * oc = (ObjectController*)model;
    PLObject * selectedObject = (PLObject*)[model selectedEntity];
    if (![selectedObject isKindOfClass:[PLObject class]])
        selectedObject = nil;
    SubentityController * sc = [selectedObject subentityModel];
    if (oc.objectMode && selectedObject)
    {
        if (matchObject)
        {
            if (!selectedObject.readOnly)
            {
                NSArray * a = [sc selectedEntities];
                BOOL matched = NO;
                for (PLEntity<PLMatching> * e in a)
                    if (e && !e.readOnly && e!=matchObject && [e conformsToProtocol:@protocol(PLMatching)] && [e matchWithEntity:matchObject])
                        matched = YES;
                matchObject = nil;
                if (matched)
                    return;
            }
        } else {
            matchObject = (PLEntity<PLMatching>*)[sc selectedEntity];
            if ([matchObject conformsToProtocol:@protocol(PLMatching)])
                return;
            matchObject = nil;
        }
    }
    NSBeep();
}


@end
