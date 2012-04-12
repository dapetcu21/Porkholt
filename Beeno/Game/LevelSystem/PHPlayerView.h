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
    ph_float lastDif;
    ph_float elapsed;
public:
    int designatedTag() { return _designatedTag; }
    void setDesignatedTag(int tag) { _designatedTag = tag; };
    
    virtual void setRotation( ph_float rot);
    void setElapsed(ph_float el) { elapsed = el; }
public:
#define PHPLAYERVIEW_INIT _designatedTag(-1), lastDif(0), elapsed(0)
    PHPlayerView() : PHView(), PHPLAYERVIEW_INIT {};
	PHPlayerView(const PHRect &frame) : PHView(frame), PHPLAYERVIEW_INIT {};
};


#endif