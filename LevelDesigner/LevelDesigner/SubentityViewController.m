//
//  SubentityViewController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "SubentityViewController.h"
#import "SubentityController.h"

@implementation SubentityViewController

- (id)init
{
    self = [super initWithTables:SubentityController_numberOfArrays 
               andPasteboardType:PLSubentityPBoardType
       andLocationPasteboardType:PLSubentityLocationPBoardType
        andPointerPasteboardType:PLSubentityPointerPBoardType];
    return self;
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
