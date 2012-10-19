/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGMOB_H
#define IGMOB_H

#include "IGDampingProp.h"

class IGMob : public IGDampingProp
{
    protected:
    public:
        IGMob(IGWorld * w);
        ~IGMob();

        const char * luaClass() { return "IGMob"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif
