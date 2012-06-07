/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "SubentityViewController.h"
#import "SubentityController.h"
#import "PLImage.h"
#import "PLFixture.h"
#import "ImageViewController.h"
#import "FixtureViewController.h"
#import "OverlayController.h"
#import "OverlayView.h"

@implementation SubentityViewController

- (id)init
{
    self = [super initWithTables:SubentityController_numberOfArrays 
               andPasteboardType:PLSubentityPBoardType
       andLocationPasteboardType:PLSubentityLocationPBoardType
        andPointerPasteboardType:PLSubentityPointerPBoardType];
    return self;
}

-(void)updateControllers
{
    PLEntity * newEntity = [model selectedEntity];
    BOOL isImage = [newEntity isKindOfClass:[PLImage class]];
    BOOL isFixture = [newEntity isKindOfClass:[PLFixture class]];

    [imageController setModel:isImage?(PLImage*)newEntity:nil];
    [fixtureController setModel:isFixture?(PLFixture*)newEntity:nil];
    
    [imageDetailView removeFromSuperview];
    [fixtureDetailView removeFromSuperview];

    OverlayView * view = nil;
    if (isImage)
        view = imageDetailView;
    else
    if (isFixture)
        view = fixtureDetailView;
    if (view)
    {
        [overlayController setRightView:view];
    }
}

-(void)selectionForArrayChanged:(NSUInteger)array
{
    [super selectionForArrayChanged:array];
    [self updateControllers];
}

-(PLTableView*)imagesView
{
    return tables[0];
}

-(PLTableView*)fixturesView
{
    return tables[1];
}

-(void)setImagesView:(PLTableView *)objectView
{
    [objectView retain];
    [tables[0] release];
    tables[0] = objectView;
}

-(void)setFixturesView:(PLTableView *)jointView
{
    [jointView retain];
    [tables[1] release];
    tables[1] = jointView;
}

@end
