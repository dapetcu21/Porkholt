/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGINPUT_H
#define IGINPUT_H

#include "IGObject.h"
#include <Porkholt/Core/PHDrawable.h>

class IGInput : public IGObject, public PHDrawableInputDelegate
{
    protected:
        void drawableRecievedEvent(PHDrawable * d, PHEvent * evt);
        int firing;
        void * moveud;
    public:
        IGInput(IGWorld * world);
        virtual ~IGInput();

        PHDrawable * loadDrawable();
        bool isFiring() { return firing != 0; }
        const char * luaClass() { return "IGInput"; }

        static void loadLuaInterface(IGScripting * s);
};

#endif
