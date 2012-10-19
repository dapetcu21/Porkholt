/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGPLAYER_H
#define IGPLAYER_H

#include "IGProp.h"

class IGPlayer : public IGProp
{
    protected:
        void configureDrawable(PHDrawable * d);
    public:
        IGPlayer(IGWorld * w);
        ~IGPlayer();

        const char * luaClass() { return "IGPlayer"; } 
        void attachedToWorld();

        static void loadLuaInterface(IGScripting * s);
};

#endif
