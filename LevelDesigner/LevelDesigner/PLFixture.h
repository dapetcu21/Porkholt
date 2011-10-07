//
//  PLFixture.h
//  LevelDesigner
//
//  Created by Marius Petcu on 9/22/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"

enum PLFixtureShapes
{
    PLFixtureRect = 0,
    PLFixtureCircle
};

@class PLProperty;
@class PLObject;
@interface PLFixture : PLEntity
{
    int shape;
    NSRect box;
    NSPoint position;
    double rotation;
    double friction;
    double density;
    double restitution;
    double radius;
    int groupIndex;
    int categoryBits;
    int maskBits;
}

@property(nonatomic,assign) int shape;
@property(nonatomic,assign) NSRect box;
@property(nonatomic,assign) NSPoint position;
@property(nonatomic,assign) double rotation;
@property(nonatomic,assign) double friction;
@property(nonatomic,assign) double density;
@property(nonatomic,assign) double restitution;
@property(nonatomic,assign) double radius;
@property(nonatomic,assign) int groupIndex;
@property(nonatomic,assign) int categoryBits;
@property(nonatomic,assign) int maskBits;

-(id)initFromProperty:(PLProperty*)prop;
-(void)writeToFile:(NSMutableString*)file;
+(NSArray*)fixturesFromProperty:(PLProperty*)prop;

@property(nonatomic,readonly) PLObject * object;

@end
