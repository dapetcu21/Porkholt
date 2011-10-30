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

class PLDotView;
class PLBezierView : public PHView {
    PLBezier * _model;
    vector<PLDotView*> dots;
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
    friend class PLDotView;
    void newDot(PLDotView * sender);
    void removeDot(PLDotView * sender);
    void changeTag(PLDotView * sender);
    void moveDot(PLDotView * sender, PHPoint delta);
    void markAsCurve(PLDotView * sender);
    void moveDotStarted(PLDotView * sender);
    void moveDotEnded(PLDotView * sender);
};

#endif
