/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGMOB_H
#define IGMOB_H

#include "IGDampingProp.h"

class IGMob : public IGDampingProp
{
    protected:
        ph_float hp;
    public:
        IGMob(IGWorld * w);
        ~IGMob();

        void setHealth(ph_float h) { hp = h; }
        ph_float health() { return hp; }
        void loseHealth(ph_float h);
        virtual void die();

        const char * luaClass() { return "IGMob"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif
