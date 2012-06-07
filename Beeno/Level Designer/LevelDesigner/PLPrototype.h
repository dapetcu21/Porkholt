/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import <Foundation/Foundation.h>

@class PLProperty;
@interface PLPrototype : NSObject
{
    PLProperty * rootProperty;
    NSArray * images;
    NSArray * fixtures;
}

@property(nonatomic,retain) PLProperty * rootProperty;
@property(nonatomic,retain) NSArray * images;
@property(nonatomic,retain) NSArray * fixtures;

@end
