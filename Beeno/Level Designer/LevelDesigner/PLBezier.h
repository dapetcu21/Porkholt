/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#import "PLEntity.h"
#include <set>
using namespace std;

class PHBezierPath;
class PLBezierDelegate;
class PLBezierView;


@interface PLBezier : PLEntity<NSCopying>
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
-(void)saveUndoState;

@end
