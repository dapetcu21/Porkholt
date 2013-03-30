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
        float upper, lower, velocity, width, epsilon, flat;
        float ftime;
        float fvel;

        list<IGWallCell*> cells;

        float f(float x);
        void fadvance(float elapsed);
        void freset();

        IGWallView * view;
        IGWallCurve * curve;

    public:
        IGWallManager(IGWorld * world);
        virtual ~IGWallManager();

#define sgetter(g, s, v) float g() { return v; } void s(float x) { v = x; }
        sgetter(upperMargin, setUpperMargin, upper);
        sgetter(lowerMargin, setLowerMargin, lower);
        sgetter(wallVelocity, setWallVelocity, velocity);
        sgetter(cellWidth, setCellWidth, width);
        sgetter(leftEpsilon, setLeftEpsilon, epsilon);
        sgetter(flatMargin, setFlatMargin, flat);

        PHDrawable * loadDrawable();
        void animate(float elapsed);
        const char * luaClass() { return "IGWallManager"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif
