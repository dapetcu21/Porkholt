/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGBULLETMANAGER_H
#define IGBULLETMANAGER_H

#include "IGObject.h"
#include "IGContactHandler.h"

class PHView;
class PHParticleView;
class b2Body;
class IGBulletParticles;

class IGBulletManager : public IGObject, public IGContactHandler
{
    public:
        IGBulletManager(IGWorld * w);
        ~IGBulletManager();

        struct bullet
        {
            int type;
            PHPoint position;
            ph_float rotation;
            int owner;

            bullet() {}
            bullet(int t, const PHPoint & pos, ph_float rot, int own) : type(t), position(pos), rotation(rot), owner(own) {}
            bullet(const bullet & o) : type(o.type), position(o.position), rotation(o.rotation), owner(o.owner) {} 
        };

        void * addBullet(const bullet & b);
        void removeBullet(void * b);
        PHDrawable * loadDrawable();

        const char * luaClass() { return "IGBulletManager"; }
        static void loadLuaInterface(IGScripting * s);
    protected:
        friend class IGBulletParticles;

        struct bullet_info
        {
            bullet b;
            b2Body * body;
            bullet_info * next, * prev; 
            ph_float time;
            bool processed;
        };
        bullet_info * bullets;
        bullet_info * lbullets;
        PHView * v;
        IGBulletParticles * pa;
        PHParticleView * pv;

        struct bullet_contact
        {
            IGObject * object;
            bullet_info * bullet;
        };
        list<bullet_contact> contacts;

        b2Body * physicsForBullet(bullet_info & b);
        void animate(ph_float elapsed);
        bool collisionCallback(bullet_info * bullet, IGObject * o);
        void beginContact(bool aBody, b2Contact * contact);
};

#endif

