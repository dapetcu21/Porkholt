/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGPROP_H
#define IGPROP_H

#include "IGObject.h"

class b2Body;
class b2MouseJoint;

class IGProp : public IGObject
{
    protected:
        PHPoint pos;
        ph_float rot;
        b2Body * body;
        b2MouseJoint * posJoint;

        void createPositionJoint();
        PHDrawable * loadDrawable();
        virtual void configureDrawable(PHDrawable * d) {};
    public:
        IGProp(IGWorld * world);
        virtual ~IGProp();
        virtual const char * luaClass() { return "IGProp"; }

        virtual void setPosition(const PHPoint & p);
        const PHPoint & position() { return pos; }
        virtual void setRotation(ph_float rot);
        ph_float rotation() { return rot; }

        static void loadLuaInterface(IGScripting * s);
        b2Body * physicsBody() { return body; }
        void setPhysicsBody(b2Body * b);

        virtual void animate(ph_float elapsed);
};

#endif
