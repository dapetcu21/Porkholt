/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGSCREENBOUNDS_H
#define IGSCREENBOUNDS_H

#include "IGProp.h"

class IGScreenBounds : public IGProp
{
    protected:
        void attachedToWorld();
    public:
        IGScreenBounds(IGWorld * w);
        ~IGScreenBounds();

        const char * luaClass() { return "IGScreenBounds"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif


