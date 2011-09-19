//
//  PLEntity.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"

@implementation PLEntity
@synthesize owner;
@synthesize selected;
@synthesize index;
@synthesize array;

-(void)setIndex:(NSUInteger)idx andArray:(NSUInteger)arr
{
    array=arr; 
    index=idx;
}

-(void)resetIndex
{
    array=index=NSNotFound;
}

@end
