//
//  PLSubEntity.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/11/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PLEntity.h"

@protocol PLSubEntity <NSObject>

-(void)move:(NSPoint)delta;
-(void)rotate:(double)ammount;

@end
