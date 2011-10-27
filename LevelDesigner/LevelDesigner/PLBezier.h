//
//  PLBezier.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "PLEntity.h"
#include <set>
using namespace std;

class PHBezierPath;
class PLBezierDelegate;
class PLBezierView;


@interface PLBezier : PLEntity
{
    PHBezierPath * curve;
    PLBezierDelegate * delegate;
    set<PLBezierView*> actors;
    BOOL fromWithin;
    NSUndoManager * undoManager;
}

@property(nonatomic,readonly) PHBezierPath * bezierPath;
@property(nonatomic,retain) NSUndoManager * undoManager;
-(void)modelChanged;
-(BOOL)isEqual:(PLBezier*)object;
-(void)writeToFile:(NSMutableString*)file;
-(void)addActor:(PLBezierView*)a;
-(void)removeActor:(PLBezierView*)a;

@end
