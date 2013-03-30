/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGMOB_H
#define IGMOB_H

#include "IGDampingProp.h"

class IGMob : public IGDampingProp
{
    protected:
        float hp;
    public:
        IGMob(IGWorld * w);
        ~IGMob();

        void setHealth(float h) { hp = h; }
        float health() { return hp; }
        void loseHealth(float h);
        virtual void die();

        const char * luaClass() { return "IGMob"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif
