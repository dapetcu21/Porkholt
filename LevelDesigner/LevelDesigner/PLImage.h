//
//  PLImage.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"

@class PLProperty;
@interface PLImage : PLEntity

+(NSArray*)imagesFromProperty:(PLProperty*)prop;

@end