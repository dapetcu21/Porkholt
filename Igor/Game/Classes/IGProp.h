/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGPROP_H
#define IGPROP_H

#include "IGObject.h"

class b2Body;

class IGProp : public IGObject
{
    protected:
        PHPoint pos;
        b2Body * body;

        virtual b2Body * loadBody() { return NULL; }
        PHDrawable * loadDrawable();
        virtual void configureDrawable(PHDrawable * d) {};
    public:
        IGProp(IGWorld * world);
        virtual ~IGProp();
        virtual const char * luaClass() { return "IGProp"; }

        void setPosition(const PHPoint & p);
        const PHPoint & position() { return pos; }

        b2Body * physicsBody() { if (!body) body = loadBody(); return body; }

        static void loadLuaInterface(IGScripting * s);
};

#endif
