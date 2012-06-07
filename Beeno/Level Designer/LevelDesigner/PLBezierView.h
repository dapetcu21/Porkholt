/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PLBEZIERVIEW_H
#define PLBEZIERVIEW_H

#import "PLBezier.h"
#include "PHTestView.h"

class PLBezierDot;
class PLBezierView : public PHView {
    PLBezier * _model;
    vector<PLBezierDot*> dots;
    bool fromWithin;
    int markedAnchor;
public:
    PLBezierView() : markedAnchor(-1), _model(NULL), fromWithin(false) { setUserInput(true); }
    ~PLBezierView();
    void setModel(PLBezier * bezier);
    PLBezier * model() { return _model; }
    
    void modelChanged();
    void setBounds(const PHRect & bounds);
    
private:
    friend class PLBezierDot;
    void newDot(PLBezierDot * sender);
    void removeDot(PLBezierDot * sender);
    void changeTag(PLBezierDot * sender);
    void moveDot(PLBezierDot * sender, PHPoint delta);
    void markAsCurve(PLBezierDot * sender);
    void moveDotStarted(PLBezierDot * sender);
    void moveDotEnded(PLBezierDot * sender);
};

#endif
