//
//  WorldController.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

@class ObjectController;
@class PLPorkholtView;
class PLWorldView;
#import "PHGeometry.h"

@interface WorldController : NSObject
{
    ObjectController * model;
    PLPorkholtView * view;
    PLWorldView * worldView;
    int type;
    NSIndexSet * initialIndexSet;
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

@end
