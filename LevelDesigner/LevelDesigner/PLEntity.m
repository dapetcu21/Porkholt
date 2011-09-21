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
@synthesize readOnly;


-(id)initFromLua:(lua_State*)L
{
    if (self = [super init])
    {
        
    }
    return self;
}

-(void)saveToFile:(NSMutableString*)file
{
    
}

-(void)setIndex:(NSUInteger)idx andArray:(NSUInteger)arr
{
    array=arr; 
    index=idx;
}

-(void)resetIndex
{
    array=index=NSNotFound;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
    
}

- (id)initWithCoder:(NSCoder *)aDecoder
{
    if (self = [self init])
    {
        
    }
    return self;
}

@end
