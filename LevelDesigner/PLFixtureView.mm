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

void PLFixtureView::modelChanged()
{
    
}


