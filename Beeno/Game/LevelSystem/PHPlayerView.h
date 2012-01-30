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
    int _designatedTag;
    double lastDif;
public:
    int designatedTag() { return _designatedTag; }
    void setDesignatedTag(int tag) { _designatedTag = tag; };
    
    virtual void setRotation( double rot);
    
public:
#define PHPLAYERVIEW_INIT _designatedTag(-1), lastDif(0)
    PHPlayerView() : PHView(), PHPLAYERVIEW_INIT {};
	PHPlayerView(const PHRect &frame) : PHView(frame), PHPLAYERVIEW_INIT {};
};


#endif