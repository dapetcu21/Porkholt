//
//  PLObjectMarker.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/11/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLOBJECTMARKER_H
#define PLOBJECTMARKER_H

#include "PHView.h"

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
