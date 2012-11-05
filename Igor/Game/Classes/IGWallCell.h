/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWALLCELL_H
#define IGWALLCELL_H

#include "IGDampingProp.h"

class b2Joint;

class IGWallCell : public IGDampingProp
{
    protected:
        void configureDrawable(PHDrawable * d);
    public:
        IGWallCell(IGWorld * w);
        ~IGWallCell();

        void attachedToWorld();
        b2Joint * weldToObject(IGObject * obj, bool last = false);

        const char * luaClass() { return "IGWallCell"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
