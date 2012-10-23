/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGPROP_H
#define IGPROP_H

#include "IGObject.h"
#include "IGContactHandler.h"

class b2Body;
class b2MouseJoint;

class IGProp : public IGObject, public IGContactHandler
{
    protected:
        PHPoint pos;
        ph_float rot;
        b2Body * body;
        b2MouseJoint * posJoint;
        bool collisions;

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
        void setCollisionEvents(bool ce) { collisions = ce; }
        bool collisionEvents() { return collisions; }

        static void loadLuaInterface(IGScripting * s);
        b2Body * physicsBody() { return body; }
        void setPhysicsBody(b2Body * b);

        virtual void animate(ph_float elapsed);
};

#endif
