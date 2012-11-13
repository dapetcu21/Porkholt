/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWALLMANAGER_H
#define IGWALLMANAGER_H

#include "IGObject.h"
#include <Porkholt/Core/PHDrawable.h>

class IGWallCell;
class IGWallView;
class IGWallCurve;

class IGWallManager : public IGObject
{
    protected:
        ph_float upper, lower, velocity, width, epsilon;
        ph_float ftime;

        list<IGWallCell*> cells;

        ph_float f(ph_float x);
        void fadvance(ph_float elapsed);
        void freset();

        IGWallView * view;
        IGWallCurve * curve;

    public:
        IGWallManager(IGWorld * world);
        virtual ~IGWallManager();

#define sgetter(g, s, v) ph_float g() { return v; } void s(ph_float x) { v = x; }
        sgetter(upperMargin, setUpperMargin, upper);
        sgetter(lowerMargin, setLowerMargin, lower);
        sgetter(wallVelocity, setWallVelocity, velocity);
        sgetter(cellWidth, setCellWidth, width);
        sgetter(leftEpsilon, setLeftEpsilon, epsilon);

        PHDrawable * loadDrawable();
        void animate(ph_float elapsed);
        const char * luaClass() { return "IGWallManager"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif
