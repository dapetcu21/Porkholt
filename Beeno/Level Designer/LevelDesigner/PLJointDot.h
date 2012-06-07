/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
