//
//  PLJointDot.h
//  LevelDesigner
//
//  Created by Marius Petcu on 11/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLJOINTDOT_H
#define PLJOINTDOT_H

#import "PLDotView.h"

@class PLJoint;
class PLJointDot : public PLDotView
{
private:
    PLJoint * model;
    bool first,dragged;
public:
    PLJoint * joint() { return model; }
    void setJoint(PLJoint * j);
    PLJointDot();
    ~PLJointDot();
    
    bool isFirst() { return first; }
    void setFirst(bool f) { first = f; }
    void jointChanged();
    void moveModel(PHPoint delta);
    void touchEvent(PHEvent * evt);
    void draw();
};

#endif
