//
//  InterfaceController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/19/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "InterfaceController.h"
#import "ObjectController.h"

@implementation InterfaceController

- (id)init
{
    self = [super initWithTables:ObjectController_numberOfArrays 
               andPasteboardType:PLObjectPBoardType
       andLocationPasteboardType:PLObjectLocationPBoardType
        andPointerPasteboardType:PLObjectPointerPBoardType];
    if (self) {
    }
    
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
    [tables[0] release];
    tables[0] = jointView;
}

@end
