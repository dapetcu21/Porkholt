//
//  PHGameView.h
//  Igor
//
//  Created by Marius Petcu on 2/17/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGAMEVIEW_H
#define PHGAMEVIEW_H

#include "PHView.h"

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
