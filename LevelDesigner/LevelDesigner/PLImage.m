//
//  PLImage.m
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLImage.h"
#import "PLProperty.h"

@implementation PLImage

- (id)initFromLua:(lua_State *)L
{
    self = [super initFromLua:L];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

+(NSArray*)imagesFromProperty:(PLProperty*)prop
{
    return [NSArray array];
}

@end
