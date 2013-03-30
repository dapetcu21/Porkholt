/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBULLET_H
#define IGBULLET_H

#include <Porkholt/Core/PHParticleAnimator.h>

class IGBulletManager;

class IGBullet
{
    protected:
        IGBulletManager * _bulletManager;
    public:
        IGBullet * next, * prev;

        IGBullet(IGBulletManager * man) : _bulletManager(man), next(NULL), prev(NULL) {}
        virtual ~IGBullet() {};
        IGBulletManager * bulletManager() { return _bulletManager; }

        virtual size_t numberOfParticles() { return 0; }
        virtual void addParticles(PHParticleAnimator::particle2D * v) = 0;
        virtual void animate(float elapsed) = 0;
        virtual void beginContact(bool aBody, b2Contact * contact) {}
};

#endif
