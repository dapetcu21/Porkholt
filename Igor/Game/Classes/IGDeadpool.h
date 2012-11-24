/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGDEADPOOL_H
#define IGDEADPOOL_H

#include "IGMob.h"

class PHImageView;

class IGDeadpool : public IGMob
{
    protected:
        void configureDrawable(PHDrawable * d);
        PHImageView * dialog;
    public:
        IGDeadpool(IGWorld * w);
        ~IGDeadpool();

        void attachedToWorld();
        void dialog1();
        void dialog2();

        const char * luaClass() { return "IGDeadpool"; } 
        static void loadLuaInterface(IGScripting * s);
};

#endif
