/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGPLAYER_H
#define IGPLAYER_H

#include "IGProp.h"

class IGPlayer : public IGProp
{
    protected:
        float hp;
        void configureDrawable(PHDrawable * d);
    public:
        IGPlayer(IGWorld * w);
        ~IGPlayer();

        void attachedToWorld();

        void setHealth(float h) { hp = h; }
        float health() { return hp; }
        void loseHealth(float h);
        void die();

        const char * luaClass() { return "IGPlayer"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
