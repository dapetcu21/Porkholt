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
#import "ObjectController.h"

@implementation WorldController

- (id)init
{
    self = [super init];
    if (self) {
    }
    return self;
}
-(PLPorkholtView*)view
{
    return view;
}

-(void)setView:(PLPorkholtView *)v
{
    [v retain];
    [view release];
    view = v;
    worldView = view.worldView;
    [self reloadViews];
}

-(ObjectController*)model
{
    return model;
}

-(void)dealloc
{
    [model release];
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
            obj.actor = new PLObjectView(obj);
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

@end
