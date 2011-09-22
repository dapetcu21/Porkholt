//
//  SubentityController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "SubentityController.h"
#import "PLImage.h"
#import "PLFixture.h"
#import "PLEntity.h"

@implementation SubentityController

- (id)initWithObject:(PLEntity*)e
{
    self = [super initWithArrays:SubentityController_numberOfArrays andPasteboardType:PLSubentityPBoardType];
    if (self)
    {
        owner = e;
    }
    return self;
}

-(Class)classForArray:(NSUInteger)array
{
    switch (array) {
        case 0:
            return [PLImage class];
        case 1:
            return [PLFixture class];
        default:
            return NULL;
    }
}
@end
