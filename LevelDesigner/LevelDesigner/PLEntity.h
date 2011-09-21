//
//  PLEntity.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/18/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import <Foundation/Foundation.h>

@class EntityController;
@interface PLEntity : NSObject<NSCoding>
{
    BOOL selected,readOnly;
    EntityController * owner;
    NSUInteger index,array;
}

@property(nonatomic,assign) BOOL selected;
@property(nonatomic,assign) BOOL readOnly;
@property(nonatomic,assign) EntityController * owner;
@property(nonatomic,assign) NSUInteger index;
@property(nonatomic,assign) NSUInteger array;

-(void)setIndex:(NSUInteger)idx andArray:(NSUInteger)arr;
-(void)resetIndex;

-(id)initFromLua:(lua_State*)L;
-(void)saveToFile:(NSMutableString*)file;

@end
