/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGSTANDARDBULLET_H
#define IGSTANDARDBULLET_H

#include "IGBullet.h"

class IGScripting;
class b2Body;
class IGObject;

class IGStandardBullet : public IGBullet
{
    protected:
        PHVector2 position;
        ph_float angle;
        int owner;
        IGObject * contact;
        b2Body * body;
    public:
        IGStandardBullet(IGBulletManager * man, const PHVector2 & pos, const PHVector2 & vel, int owner = 1);
        ~IGStandardBullet();

        size_t numberOfParticles() { return 1; }
        void addParticles(PHParticleAnimator::particle2D * v);
        void animate(ph_float elapsed);

        void beginContact(bool aBody, b2Contact * contact);

        static void loadLuaInterface(IGScripting * s);
};

#endif
