/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGCONTACTHANDLER_H
#define IGCONTACTHANDLER_H

class b2Contact;
class b2Manifold;
class b2ContactImpulse;

class IGContactHandler
{
    public:
        virtual void beginContact(bool aBody, b2Contact* contact) {} 
        virtual void endContact(bool aBody, b2Contact* contact) {}
        virtual void preSolve(bool aBody, b2Contact* contact, const b2Manifold* oldManifold) {}
        virtual void postSolve(bool aBody, b2Contact* contact, const b2ContactImpulse* impulse) {}
};

#endif

