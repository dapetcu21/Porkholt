/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGDAMPINGPROP_H
#define IGDAMPINGPROP_H

#include "IGProp.h"

class IGDampingProp : public IGProp
{
    public:
        IGDampingProp(IGWorld * w);
        ~IGDampingProp();

        virtual const char * luaClass() { return "IGDampingProp"; }

        static void loadLuaInterface(IGScripting * s);
};

#endif
