//
//  PLWorldView.h
//  LevelDesigner
//
//  Created by Marius Petcu on 10/7/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PLWORLDVIEW_H
#define PLWORLDVIEW_H

#include "PHView.h"
#include "PHGeometry.h"

#ifdef __OBJC__
@class WorldController;
#else
#define WorldController void
#endif

class PLWorldView : public PHView
{
protected:
    WorldController * worldController;
    void touchEvent(PHEvent * event);
    PHRect dragRect;
    PHPoint startPoint;
    void draw();
    bool rotating;
public:
    PLWorldView();
    WorldController * controller() { return worldController; }
    void setController(WorldController *c) { worldController = c; }
};

#endif
