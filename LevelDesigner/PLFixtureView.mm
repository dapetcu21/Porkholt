//
//  PLFixtureView.mm
//  LevelDesigner
//
//  Created by Marius Petcu on 10/10/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLFixtureView.h"
#import "PLFixture.h"
#import "PLObjectView.h"
#import "SubentityController.h"
#import "ObjectController.h"

PLFixtureView::PLFixtureView(PLFixture * _model) : model(_model)
{
    [model retain];
    [model setActor:this];
    modelChanged();
}

PLFixtureView::~PLFixtureView()
{
    [model setActor:NULL];
    [model release];
}

bool PLFixtureView::intersectsRect(PHView * base, const PHRect & rect)
{
    return PLObjectView::rectsIntersect(base, rect, base, _bounds, this);
}

bool PLFixtureView::intersectsPoint(const PHPoint & pnt)
{
    return PHPointInRect(pnt, _bounds);
}

void PLFixtureView::modelChanged()
{
    
}

bool PLFixtureView::objectMode()
{
    return ([(ObjectController*)[[(SubentityController*)[model owner] object] owner] objectMode]);
}


