/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBACKGROUNDPARTICLES_H
#define IGBACKGROUNDPARTICLES_H

#include "IGObject.h"

class IGBackgroundParticles : public IGObject
{
    protected:
        PHDrawable * loadDrawable();
    public:
        IGBackgroundParticles(IGWorld * w);
        ~IGBackgroundParticles();

        const char * luaClass() { return "IGBackgroundParticles"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
