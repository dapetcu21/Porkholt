/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PLOBJECTMARKER_H
#define PLOBJECTMARKER_H

#include <Porkholt/Core/PHView.h>

class PLObjectMarker : public PHView
{
protected:
    void draw();
    void touchEvent(PHEvent * event);
public:
    PLObjectMarker(const PHRect frame) : PHView(frame) {}
    PLObjectMarker() : PHView() {}
};

#endif
