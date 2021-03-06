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
        float rot;
        b2Body * body;
        b2MouseJoint * posJoint;
        bool collisions;

        void createPositionJoint();
        PHDrawable * loadDrawable();
        virtual void configureDrawable(PHDrawable * d) {};

        void beginContact(bool aBody, b2Contact * contact);
        void endContact(bool aBody, b2Contact* contact);
        void _beginContact(PHObject * sender, IGObject * contact);
        void _endContact(PHObject * sender, IGObject * contact);
        //void preSolve(bool aBody, b2Contact* contact, const b2Manifold* oldManifold);
        //void postSolve(bool aBody, b2Contact* contact, const b2ContactImpulse* impulse);

    public:
        IGProp(IGWorld * world);
        virtual ~IGProp();
        virtual const char * luaClass() { return "IGProp"; }

        virtual void setPosition(const PHPoint & p);
        const PHPoint & position() { return pos; }
        virtual void setRotation(float rot);
        float rotation() { return rot; }

        PHPoint worldPoint(const PHPoint & p);
        PHPoint localPoint(const PHPoint & p);
        
        void setCollisionEvents(bool ce) { collisions = ce; }
        bool collisionEvents() { return collisions; }

        static void loadLuaInterface(IGScripting * s);
        b2Body * physicsBody() { return body; }
        void setPhysicsBody(b2Body * b);

        virtual void animate(float elapsed);
};

#endif
