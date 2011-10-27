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
#include "PHTestView.h"

class PLBezierView : public PHTestView {
    PLBezier * _model;
    
public:
    PLBezierView() : _model(NULL) {}
    ~PLBezierView();
    void setModel(PLBezier * bezier);
    PLBezier * model() { return _model; }
    
    void modelChanged();
};

#endif
