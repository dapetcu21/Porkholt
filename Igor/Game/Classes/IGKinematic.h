/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGKINEMATIC_H
#define IGKINEMATIC_H

#include "IGProp.h"

class IGKinematic : public IGProp
{
    public:
        IGKinematic(IGWorld * w);
        ~IGKinematic();

        void attachedToWorld();

        const char * luaClass() { return "IGKinematic"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
