/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

@class ObjectController;
@class PLPorkholtView;
@class PLObject;
class PLWorldView;
#import "PHGeometry.h"

@interface WorldController : NSObject
{
    ObjectController * model;
    PLPorkholtView * view;
    PLWorldView * worldView;
    int type;
    NSIndexSet * initialIndexSet, * initialImages, * initialFixtures;
    
    PHPoint dragged;
    double rotated;
    NSMutableArray * movedObjects;
    NSMutableArray * rotatedObjects;
    PLObject * selectedObject;
}

-(void)objectsChanged;
-(void)jointsChanged;
-(void)objectSelectionChanged;
-(void)jointSelectionChanged;
-(void)reloadViews;
@property(nonatomic,retain) IBOutlet PLPorkholtView * view;
@property(nonatomic,retain) IBOutlet ObjectController * model;

-(void)startSelectionOfType:(int)type atPoint:(PHPoint)p;
-(void)moveSelection:(PHRect)area;
-(void)endSelection:(PHRect)area;

-(void)startMoving;
-(void)startRotating;
-(void)move:(PHPoint)ammount;
-(void)rotate:(double)ammount;
-(void)stopMoving;
-(void)stopRotating;

-(void)flagsChanged;

-(PLObject*)selectedObject;

@end
