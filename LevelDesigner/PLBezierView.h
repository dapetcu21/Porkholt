//
//  PLBezierView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLBEZIERVIEW_H
#define PLBEZIERVIEW_H

#import "PLBezier.h"
#include "PHView.h"

class PLBezierView : public PHView {
    PLBezier * _model;
public:
    PLBezierView() : _model(NULL) {}
    ~PLBezierView() { [_model release]; }
    void setModel(PLBezier * bezier);
    PLBezier * model() { return _model; }
    
    void modelChanged();
};

#endif
