//
//  PLBezierView.cpp
//  LevelDesigner
//
//  Created by Marius Petcu on 10/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLBezierView.h"
#include "PLBezier.h"

void PLBezierView::setModel(PLBezier * bezier)
{
    if (bezier!=_model) return;
    [_model removeActor:this];
    [bezier addActor:this];
    
    [bezier retain];
    [_model release];
    _model = bezier;
    modelChanged();
}

PLBezierView::~PLBezierView() { 
    [_model removeActor:this];
    [_model release]; 
}

void PLBezierView::modelChanged()
{
    
}