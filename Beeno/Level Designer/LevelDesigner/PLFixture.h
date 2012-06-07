/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PLSubEntity.h"
#import "PLMatching.h"

enum PLFixtureShapes
{
    PLFixtureRect = 0,
    PLFixtureCircle,
    PLFixtureFreestyle
};

@class PLProperty;
@class PLObject;
@class FixtureViewController;
@class PLBezier;

#ifdef __cplusplus
class PLFixtureView;
#else
#define PLFixtureView void
#endif

@interface PLFixture : PLEntity<PLSubEntity,PLMatching>
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
    
    FixtureViewController * viewController;
    PLFixtureView * actor;
    PLBezier * bezierCurve;
    NSUInteger bezierCurveIndex;
}

@property(nonatomic,retain) PLBezier * bezierCurve;
@property(nonatomic,assign) NSUInteger bezierCurveIndex;
@property(nonatomic,assign) int shape;
@property(nonatomic,assign) NSRect box;
@property(nonatomic,assign) NSPoint position;
@property(nonatomic,assign) double rotation;
@property(nonatomic,assign) double radius;

@property(nonatomic,assign) double friction;
@property(nonatomic,assign) double density;
@property(nonatomic,assign) double restitution;
@property(nonatomic,assign) int groupIndex;
@property(nonatomic,assign) int categoryBits;
@property(nonatomic,assign) int maskBits;

-(id)initFromProperty:(PLProperty*)prop;
-(void)writeToFile:(NSMutableString*)file;
+(NSArray*)fixturesFromProperty:(PLProperty*)prop;

@property(nonatomic,readonly) PLObject * object;
@property(nonatomic,assign) FixtureViewController * viewController;
@property(nonatomic,assign) PLFixtureView * actor;

@end
