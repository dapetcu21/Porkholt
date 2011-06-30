//
//  PHPlayerView.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHPLAYERVIEW_H
#define PHPLAYERVIEW_H

#include "PHView.h"

class PHPlayerView : public PHView
{
private:
    PHView * _designatedView;
    double lastDif;
public:
    PHView * designatedView() { return _designatedView; }
    void setDesignatedView(PHView * view) { _designatedView = view; };
    
    virtual void setRotation( double rot);
    
public:
#define PHPLAYERVIEW_INIT _designatedView(NULL), lastDif(0)
    PHPlayerView() : PHView(), PHPLAYERVIEW_INIT {};
	PHPlayerView(const PHRect &frame) : PHView(frame), PHPLAYERVIEW_INIT {};
};


#endif