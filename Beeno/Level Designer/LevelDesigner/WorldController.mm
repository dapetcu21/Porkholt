/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
#import "SubentityController.h"
#import "PLImage.h"
#import "PLFixture.h"
#import "PLImageView.h"
#import "PLFixtureView.h"
#import "PLSubEntity.h"
#import "PLJointDot.h"
#import "PLJoint.h"

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
    [selectedObject release];
    [initialImages release];
    [initialFixtures release];
    [initialIndexSet release];
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
    for (PLJoint * jnt in [model joints])
    {
        if (jnt.actor1)
            (jnt.actor1)->retain();
        if (jnt.actor2)
            (jnt.actor2)->retain();
    }
    worldView->removeAllChildren();
    for (PLObject * obj in [model objects])
    {
        if (!obj.actor)
        {
            obj.actor = new PLObjectView(obj);
            obj.actor->setController(self);
        }
        (obj.actor)->removeAllJoints();
        worldView->addChild(obj.actor);
    }
    for (PLJoint * jnt in [model joints])
    {
        if (!jnt.actor1)
        {
            PLJointDot * d = new PLJointDot();
            d->setFirst(true);
            d->setJoint(jnt);
        }
        jnt.actor1->jointChanged();
        if (jnt.worldCoord)
        {
            if (((ObjectController*)jnt.owner).showJoints)
                worldView->addChild(jnt.actor1);
        } else
            if (jnt.body1.actor)
                (jnt.body1.actor)->addJoint(jnt.actor1);
        if ((jnt.type) == PLDistanceJoint)
        {
            if (!jnt.actor2)
            {
                PLJointDot * d = new PLJointDot();
                d->setFirst(false);
                d->setJoint(jnt);
            }
            jnt.actor2->jointChanged();
            if (jnt.worldCoord)
            {
                if (((ObjectController*)jnt.owner).showJoints)
                    worldView->addChild(jnt.actor2);
            } else
                if (jnt.body2.actor)
                    (jnt.body2.actor)->addJoint(jnt.actor2);
        }
    }
    for (PLObject * obj in [model objects])
        if (obj.actor)
            (obj.actor)->release();
    for (PLJoint * jnt in [model joints])
    {
        if (jnt.actor1)
            (jnt.actor1)->release();
        if (jnt.actor2)
            (jnt.actor2)->release();
    }
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
    if (model.objectMode)
    {
        type = t;
        if (type==0)
            [[selectedObject subentityModel] clearSelection];
        initialImages = [[[selectedObject subentityModel] selectionForArray:0] copy];
        initialFixtures = [[[selectedObject subentityModel] selectionForArray:1] copy];
    } else 
    {
        type = t;
        if (type==0)
            [model clearSelection];
        initialIndexSet = [[model selectionForArray:0] copy];
    }
}

-(void)moveSelection:(PHRect)area
{
    if (type<0)
        return;
    if (model.objectMode)
    {
        NSMutableIndexSet * is0 = [NSMutableIndexSet indexSet];
        NSMutableIndexSet * is1 = [NSMutableIndexSet indexSet];
        SubentityController * sc = [selectedObject subentityModel];
        for (PLImage * obj in [sc images])
            if (obj.actor && obj.actor->intersectsRect(worldView,area))
                [is0 addIndex:[sc indexForEntity:obj inArray:0]];
        for (PLFixture * obj in [sc fixtures])
            if (obj.actor && obj.actor->intersectsRect(worldView,area))
                [is1 addIndex:[sc indexForEntity:obj inArray:1]];
        [sc beginCommitGrouping];
        [sc clearSelection];
        if (type==1)
        {
            [is0 addIndexes:initialImages];
            [is1 addIndexes:initialFixtures];
        }
        else
            if (type==2)
            {
                NSMutableIndexSet * iss0 = [[initialImages mutableCopy] autorelease];
                [iss0 removeIndexes:is0];
                is0 = iss0;
                NSMutableIndexSet * iss1 = [[initialFixtures mutableCopy] autorelease];
                [iss1 removeIndexes:is1];
                is1 = iss1;
            }
        [sc insertIndexes:is0 inSelectionForArray:0];
        [sc insertIndexes:is1 inSelectionForArray:1];
        [sc endCommitGrouping];
    } else
    {
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
}

-(void)endSelection:(PHRect)area
{
    [initialImages release];
    [initialFixtures release];
    [initialIndexSet release];
    initialIndexSet = initialImages = initialFixtures = nil;
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
    if (model.objectMode)
    {
        dragged = PHOriginPoint;
        [[model undoManager] disableUndoRegistration];
        [movedObjects release];
        movedObjects = [[NSMutableArray alloc] init];
        for (PLEntity * obj in [[selectedObject subentityModel] selectedEntities])
            if (!obj.readOnly)
                [movedObjects addObject:obj];
    } else 
    {
        dragged = PHOriginPoint;
        [[model undoManager] disableUndoRegistration];
        [movedObjects release];
        movedObjects = [[NSMutableArray alloc] init];
        for (PLObject * obj in [model selectedEntitiesForArray:0])
            if (!obj.readOnly)
                [movedObjects addObject:obj];
    }
}

-(void)startRotating
{
    if (model.objectMode)
    {
        rotated = 0;
        [[model undoManager] disableUndoRegistration];
        [rotatedObjects release];
        rotatedObjects = [[NSMutableArray alloc] init];
        for (PLEntity * obj in [[selectedObject subentityModel] selectedEntities])
            if (!obj.readOnly)
                [rotatedObjects addObject:obj];
    } else
    {
        rotated = 0;
        [[model undoManager] disableUndoRegistration];
        [rotatedObjects release];
        rotatedObjects = [[NSMutableArray alloc] init];
        for (PLObject * obj in [model selectedEntitiesForArray:0])
            if (!obj.readOnly)
                [rotatedObjects addObject:obj];
    }
}

-(void)move:(PHPoint)ammount
{
    if (model.objectMode)
    {
        for (PLEntity<PLSubEntity> * obj in movedObjects)
            [obj move:NSPointFromPHPoint(ammount)];
    } else
    {
        for (PLObject * obj in movedObjects)
            [obj move:NSPointFromPHPoint(ammount)];
    }
    dragged+=ammount;
}

-(void)rotate:(double)ammount
{
    if (model.objectMode)
    {
        for (PLEntity<PLSubEntity> * obj in rotatedObjects)
            [obj rotate:ammount];
    } else
    {
        for (PLObject * obj in rotatedObjects)
            [obj rotate:ammount];
    }
    rotated+=ammount;
}

-(PLObject*)selectedObject
{
    return selectedObject;
}

-(void)stopMoving
{
    if (model.objectMode)
    {
        for (PLEntity<PLSubEntity> * obj in movedObjects)
            [obj move:NSPointFromPHPoint(dragged*(-1))];
        [[model undoManager] enableUndoRegistration];
        for (PLEntity<PLSubEntity> * obj in movedObjects)
            [obj move:NSPointFromPHPoint(dragged)];
        [movedObjects release];
        movedObjects = nil;
    } else
    {
        for (PLObject * obj in movedObjects)
            [obj move:NSPointFromPHPoint(dragged*(-1))];
        [[model undoManager] enableUndoRegistration];
        for (PLObject * obj in movedObjects)
            [obj move:NSPointFromPHPoint(dragged)];
        [movedObjects release];
        movedObjects = nil;
    }
}

-(void)stopRotating
{
    if (model.objectMode)
    {
        for (PLEntity<PLSubEntity> * obj in rotatedObjects)
            [obj rotate:-rotated];
        [[model undoManager] enableUndoRegistration];
        for (PLEntity<PLSubEntity> * obj in rotatedObjects)
            [obj rotate:rotated];
        [rotatedObjects release];
        rotatedObjects = nil;
    } else
    {
        for (PLObject * obj in rotatedObjects)
            [obj rotate:-rotated];
        [[model undoManager] enableUndoRegistration];
        for (PLObject * obj in rotatedObjects)
            [obj rotate:rotated];
        [rotatedObjects release];
        rotatedObjects = nil;
    }
}

-(void)flagsChanged
{
    [selectedObject release];
    selectedObject = (PLObject*)[model selectedEntity];
    if (![selectedObject isKindOfClass:[PLObject class]])
        selectedObject = nil;
    [selectedObject retain];
    for (PLObject * obj in [model objects])
        if (obj.actor)
            (obj.actor)->flagsChanged();
    [self reloadViews];
}

@end
