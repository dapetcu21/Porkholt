/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBASICMOB_H
#define IGBASICMOB_H

#include "IGMob.h"

class IGBasicMob : public IGMob
{
    protected:
        void attachedToWorld();
        void configureDrawable(PHDrawable * d);
    public:
        IGBasicMob(IGWorld * w);
        ~IGBasicMob();

        const char * luaClass() { return "IGBasicMob"; }
        static void loadLuaInterface(IGScripting * s);
};

#endif
