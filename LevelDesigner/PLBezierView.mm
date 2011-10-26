//
//  PLBezierView.cpp
//  LevelDesigner
//
//  Created by Marius Petcu on 10/26/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PLBezierView.h"

void PLBezierView::setModel(PLBezier * bezier)
{
    [bezier retain];
    [_model release];
    _model = bezier;
    modelChanged();
}

void PLBezierView::modelChanged()
{
    
}