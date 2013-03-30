/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGBulletManager.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include "IGBullet.h"
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHLua.h>

IGSCRIPTING_REGISTERCLASS("IGBulletManager", IGBulletManager)

class IGBulletParticles : public PHParticleAnimator
{
    protected:
        IGBulletManager * man;
        particles part;
    public:
        IGBulletParticles(IGBulletManager * im) : man(im)
        {
            part.n = 0;
            part.v2d = NULL;
            part.type = particleType2D;
        }
        ~IGBulletParticles()
        {
            free(part.v2d);
        }

        particles * calculatedParticles();
        void advanceAnimation(float elapsed) {}
};

IGBulletManager::IGBulletManager(IGWorld * w) : IGObject(w), bullets(NULL)
{
    v = new PHView(w->view()->bounds());
    pv = new PHParticleView(v->bounds());
    pa = new IGBulletParticles(this);
    pv->setParticleAnimator(pa);
    pv->setImage(w->gameManager()->imageNamed("bullet"));
    v->addChild(pv);
}

IGBulletManager::~IGBulletManager()
{
    pa->release();
    pv->release();
    v->release();
}

void IGBulletManager::addBullet(IGBullet * b)
{
    b->next = bullets;
    if (b->next)
        b->next->prev = b;
    b->prev = NULL;
    bullets = b;
}

void IGBulletManager::removeBullet(IGBullet * i)
{
    if (i->next)
        i->next->prev = i->prev;
    if (i->prev)
        i->prev->next = i->next;
    if (bullets == i)
        bullets = i->next;
    delete i;
}

PHParticleAnimator::particles * IGBulletParticles::calculatedParticles()
{
    int n = 0;
    for (IGBullet * p = man->bullets; p; p = p->next)
        n += p->numberOfParticles();

    part.n = n;
    part.type = particleType2D;
    part.v2d = (particle2D*)realloc(part.v2d, sizeof(particle2D)*n);
    particle2D * v = part.v2d;

    for (IGBullet * p = man->bullets; p; p = p->next)
    {
        p->addParticles(v);
        v += p->numberOfParticles();
    }

    return &part;
}

PHDrawable * IGBulletManager::loadDrawable()
{
    return (PHDrawable*)v->retain();
}

void IGBulletManager::animate(float elapsed)
{
    IGBullet * nx;
    for (IGBullet * p = bullets; p; p = nx)
    {
        nx = p->next;
        p->animate(elapsed);
    }
}

void IGBulletManager::beginContact(bool aBody, b2Contact * contact)
{
    b2Fixture * f = aBody ? contact->GetFixtureA() : contact->GetFixtureB();
    ((IGBullet*)f->GetUserData())->beginContact(aBody, contact);
}


//--- Lua Interface ---

static int IGBulletManager_addBullet(lua_State * L)
{
    IGBulletManager * m = (IGBulletManager*)PHLuaThisPointer(L);
    IGBullet * b = (IGBullet*)lua_touserdata(L, 2);
    m->addBullet(b); 
    return 0;
}

static int IGBulletManager_removeBullet(lua_State * L)
{
    IGBulletManager * m = (IGBulletManager*)PHLuaThisPointer(L);
    if (lua_isuserdata(L, 2))
        m->removeBullet((IGBullet*)lua_touserdata(L, 2));
    return 0;
}

void IGBulletManager::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBulletManager");

    PHLuaAddMethod(IGBulletManager, addBullet);
    PHLuaAddMethod(IGBulletManager, removeBullet);

    lua_pop(L, 1);
}


