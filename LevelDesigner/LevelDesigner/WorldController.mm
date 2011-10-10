//
//  WorldController.m
//  LevelDesigner
//
//  Created by Marius Petcu on 10/5/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#import "WorldController.h"
#import "PLPorkholtView.h"
#import "PHView.h"
#import "PLObject.h"
#import "PLObjectView.h"
#import "PLWorldView.h"
#import "ObjectController.h"
#import "PHGameManager.h"
#import "PHScrollerView.h"
#import "PHEventHandler.h"

@implementation WorldController

- (id)init
{
    self = [super init];
    if (self) {
        worldView = new PLWorldView();
        worldView->setFrame(PHRect(0,0,100,100));
        worldView->setController(self);
        worldView->setUserInput(true);
    }
    return self;
}
-(PLPorkholtView*)view
{
    return view;
}

-(ObjectController*)model
{
    return model;
}

-(void)setView:(PLPorkholtView *)v
{
    if (view)
        view.gameManager->eventHandler()->unregisterViewForMultitouchEvents(worldView);
    [v retain];
    [view release];
    view = v;
    ((PHScrollerView*)view.gameManager->rootView())->setContentView(worldView);
    view.gameManager->eventHandler()->registerViewForMultitouchEvents(worldView);
    [self reloadViews];
}

-(void)dealloc
{
    [model release];
    view.gameManager->eventHandler()->unregisterViewForMultitouchEvents(worldView);
    worldView->release();
    [super dealloc];
}

-(void)reloadViews
{
    if (!worldView) return;
    for (PLObject * obj in [model objects])
        if (obj.actor)
            (obj.actor)->retain();
    worldView->removeAllSubviews();
    for (PLObject * obj in [model objects])
    {
        if (!obj.actor)
        {
            obj.actor = new PLObjectView(obj);
            obj.actor->setController(self);
        }
        worldView->addSubview(obj.actor);
    }
    for (PLObject * obj in [model objects])
        if (obj.actor)
            (obj.actor)->release();
    
}

-(void)setModel:(ObjectController *)m
{
    [m retain];
    [model release];
    model = m;
    [self reloadViews];
}

-(void)objectsChanged
{
    [self reloadViews];
}

-(void)jointsChanged
{
    [self reloadViews];
}

-(void)objectSelectionChanged
{
    const list<PHView*> & views = worldView->subViews();
    list<PHView*>::const_iterator i;
    for (i=views.begin(); i!=views.end(); i++)
    {
        PLObjectView * ov = dynamic_cast<PLObjectView*>(*i);
        if (ov)
            ov->setSelected(false);
    }
    for (PLObject * obj in [model selectedEntitiesForArray:0])
        [obj actor]->setSelected(true);
}

-(void)jointSelectionChanged
{
    
}

-(void)startSelectionOfType:(int)t atPoint:(PHPoint)p
{
    type = t;
    if (type==0)
        [model clearSelection];
    initialIndexSet = [[model selectionForArray:0] copy];
}

-(void)moveSelection:(PHRect)area
{
    if (type<0)
        return;
    NSMutableIndexSet * is = [NSMutableIndexSet indexSet];
    for (PLObject * obj in [model objects])
        if (obj.actor && obj.actor->intersectsRect(area))
            [is addIndex:[model indexForEntity:obj inArray:0]];
    [model beginCommitGrouping];
    [model clearSelection];
    if (type==1)
        [is addIndexes:initialIndexSet];
    else
    if (type==2)
    {
        NSMutableIndexSet * iss = [[initialIndexSet mutableCopy] autorelease];
        [iss removeIndexes:is];
        is = iss;
    }
    [model insertIndexes:is inSelectionForArray:0];
    [model endCommitGrouping];
}

-(void)endSelection:(PHRect)area
{
    [initialIndexSet release];
    initialIndexSet = nil;
    type = -1;
}

inline static NSPoint NSPointFromPHPoint(const PHPoint & p)
{
    NSPoint pn;
    pn.x = p.x;
    pn.y = p.y;
    return pn;
}

-(void)startMoving
{
    dragged = PHOriginPoint;
    [[model undoManager] disableUndoRegistration];
    [movedObjects release];
    movedObjects = [[NSMutableArray alloc] init];
    for (PLObject * obj in [model selectedEntitiesForArray:0])
        if (!obj.readOnly)
            [movedObjects addObject:obj];
}

-(void)startRotating
{
    rotated = 0;
    [[model undoManager] disableUndoRegistration];
    [rotatedObjects release];
    rotatedObjects = [[NSMutableArray alloc] init];
    for (PLObject * obj in [model selectedEntitiesForArray:0])
        if (!obj.readOnly)
            [rotatedObjects addObject:obj];
}

-(void)move:(PHPoint)ammount
{
    for (PLObject * obj in movedObjects)
        [obj move:NSPointFromPHPoint(ammount)];
    dragged+=ammount;
}

-(void)rotate:(double)ammount
{
    for (PLObject * obj in rotatedObjects)
        [obj rotate:ammount];
    rotated+=ammount;
}

-(void)stopMoving
{
    for (PLObject * obj in movedObjects)
        [obj move:NSPointFromPHPoint(dragged*(-1))];
    [[model undoManager] enableUndoRegistration];
    for (PLObject * obj in movedObjects)
        [obj move:NSPointFromPHPoint(dragged)];
    [movedObjects release];
    movedObjects = nil;
}

-(void)stopRotating
{
    for (PLObject * obj in rotatedObjects)
        [obj rotate:-rotated];
    [[model undoManager] enableUndoRegistration];
    for (PLObject * obj in rotatedObjects)
        [obj rotate:rotated];
    [rotatedObjects release];
    rotatedObjects = nil;
}

-(void)flagsChanged
{
    
}

@end
