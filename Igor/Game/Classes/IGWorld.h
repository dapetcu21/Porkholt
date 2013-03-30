/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef IGWORLD_H
#define IGWORLD_H

#include <Porkholt/Core/PHAnimator.h>

class PHGameManager;
class PHDirectory;
class IGObject;
class PHView;
class b2World;
class b2Body;
class IGContactListener;
class IGLevelController;

class IGWorld : public PHAnimator
{
    protected:
        list<IGObject*> _objects;
        PHView * _view;
        PHGameManager * gm;
        PHDirectory * dir;
        b2World * phyWorld;
        b2Body * ground;
        IGContactListener * cl;
        IGLevelController * lc;
    public:
        IGWorld(PHGameManager * gm, PHDirectory * dir, const PHRect & size, IGLevelController * lc);
        ~IGWorld();

        enum collisionBits
        {
            collisionPlayer = 1<<0,
            collisionMobs = 1<<1,
            collisionEnv = 1<<2,
            collisionPlayerBullets = 1<<3,
            collisionMobBullets = 1<<4,
            collisionAll = collisionPlayer | collisionMobs | collisionEnv | collisionPlayerBullets | collisionMobBullets,
            collisionAllMob = collisionPlayer | collisionMobs | collisionEnv | collisionPlayerBullets,
            collisionAllPlayer = collisionPlayer | collisionMobs | collisionEnv | collisionMobBullets,
            collisionAllEnv = collisionAll,
            collisionAllPlayerBullets = collisionMobs | collisionEnv,
            collisionAllMobBullets = collisionPlayer | collisionEnv
        };

        const list<IGObject*> & objects() { return _objects; }
        void insertObject(IGObject * obj, bool before = true, IGObject * ref = NULL);
        void removeObject(IGObject * obj);
        PHView * view() { return _view; }
        b2World * physicsWorld() { return phyWorld; }
        b2Body * groundBody() { return ground; }

        IGLevelController * levelController() { return lc; }

        PHGameManager * gameManager() { return gm; }
        
        void advanceAnimation(float elapsed);
};

#endif
