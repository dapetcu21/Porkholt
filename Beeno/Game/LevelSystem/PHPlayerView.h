/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPLAYERVIEW_H
#define PHPLAYERVIEW_H

#include <Porkholt/Core/PHView.h>

class PHPlayerView : public PHView
{
private:
    int _designatedTag;
    float lastDif;
    float elapsed;
public:
    int designatedTag() { return _designatedTag; }
    void setDesignatedTag(int tag) { _designatedTag = tag; };
    
    virtual void setRotation( float rot);
    void setElapsed(float el) { elapsed = el; }
public:
#define PHPLAYERVIEW_INIT _designatedTag(-1), lastDif(0), elapsed(0)
    PHPlayerView() : PHView(), PHPLAYERVIEW_INIT {};
	PHPlayerView(const PHRect &frame) : PHView(frame), PHPLAYERVIEW_INIT {};
};


#endif
