/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGHOMINGBULLET_H
#define IGHOMINGBULLET_H

#include "IGBullet.h"

class IGScripting;
class b2Body;
class IGObject;

class IGHomingBullet : public IGBullet
{
    protected:
        PHVector2 position;
        ph_float angle;
        int owner;
        IGObject * contact;
        IGObject * target;
        b2Body * body;
    public:
        IGHomingBullet(IGBulletManager * man, const PHVector2 & pos, const PHVector2 & vel, IGObject * target, int owner = 1);
        ~IGHomingBullet();

        size_t numberOfParticles() { return 1; }
        void addParticles(PHParticleAnimator::particle2D * v);
        void animate(ph_float elapsed);

        void beginContact(bool aBody, b2Contact * contact);

        static void loadLuaInterface(IGScripting * s);
};

#endif
