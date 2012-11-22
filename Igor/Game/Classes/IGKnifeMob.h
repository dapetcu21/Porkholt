/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGKNIFEMOB_H
#define IGKNIFEMOB_H

#include "IGMob.h"

class IGKnifeMob : public IGMob
{
    protected:
        void configureDrawable(PHDrawable * d);
    public:
        IGKnifeMob(IGWorld * w);
        ~IGKnifeMob();

        void attachedToWorld();

        const char * luaClass() { return "IGKnifeMob"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
