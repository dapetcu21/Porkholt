/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGAMEVIEW_H
#define PHGAMEVIEW_H

#include <Porkholt/Core/PHView.h>

class PHGameView : public PHView
{
protected:
    PHEvent * t1;
    PHEvent * t2;
    PHPoint p1;
    PHPoint p2;
    
    void touchEvent(PHEvent * evt);
    
public:
    PHGameView();
    PHGameView(const PHRect & f);
    
    bool touchesLeft() { return t1!=NULL; }
    bool touchesRight() { return t2!=NULL; }
    const PHPoint & leftTouch() { return p1; }
    const PHPoint & rightTouch() { return p2; }
};

#endif
