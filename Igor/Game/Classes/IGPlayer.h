/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGPLAYER_H
#define IGPLAYER_H

#include "IGProp.h"

class IGPlayer : public IGProp
{
    protected:
        ph_float hp;
        void configureDrawable(PHDrawable * d);
    public:
        IGPlayer(IGWorld * w);
        ~IGPlayer();

        void attachedToWorld();

        void setHealth(ph_float h) { hp = h; }
        ph_float health() { return hp; }
        void loseHealth(ph_float h);
        void die();

        const char * luaClass() { return "IGPlayer"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
