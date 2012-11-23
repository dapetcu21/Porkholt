/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBULLETMANAGER_H
#define IGBULLETMANAGER_H

#include "IGObject.h"
#include "IGContactHandler.h"

class PHView;
class PHParticleView;
class b2Body;
class IGBulletParticles;
class IGBullet;

class IGBulletManager : public IGObject, public IGContactHandler
{
    public:
        IGBulletManager(IGWorld * w);
        ~IGBulletManager();

        void addBullet(IGBullet * b);
        void removeBullet(IGBullet * b);
        PHDrawable * loadDrawable();

        const char * luaClass() { return "IGBulletManager"; }
        static void loadLuaInterface(IGScripting * s);

        void beginContact(bool aBody, b2Contact * contact);
    protected:
        friend class IGBulletParticles;

        IGBullet * bullets;
        PHView * v;
        IGBulletParticles * pa;
        PHParticleView * pv;

        void animate(ph_float elapsed);
};

#endif

