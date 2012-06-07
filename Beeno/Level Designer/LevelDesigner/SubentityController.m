/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "SubentityController.h"
#import "PLImage.h"
#import "PLFixture.h"
#import "PLEntity.h"
#import "PLObject.h"

@implementation SubentityController

-(NSUndoManager*)undoManager
{
    return [((PLObject*)owner) undoManager];
}

- (id)initWithObject:(PLEntity*)e
{
    self = [super initWithArrays:SubentityController_numberOfArrays andPasteboardType:PLSubentityPBoardType];
    if (self)
    {
        owner = e;
    }
    return self;
}

-(PLObject*)object
{
    return (PLObject*)owner;
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

-(NSArray*)images
{
    return [self arrayAtIndex:0];
}

-(NSArray*)fixtures
{
    return [self arrayAtIndex:1];
}

-(void)arrayChanged:(NSUInteger)array
{
    [super arrayChanged:array];
    [[self object] objectChanged];
}

@end
