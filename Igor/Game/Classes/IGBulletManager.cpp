/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGBulletManager.h"
#include "IGScripting.h"
#include "IGWorld.h"
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

        particles * calculatedParticles();
        void advanceAnimation(ph_float elapsed) {}
};

IGBulletManager::IGBulletManager(IGWorld * w) : IGObject(w), bullets(NULL), lbullets(NULL)
{
    v = new PHView(w->view()->bounds());
    pv = new PHParticleView(v->bounds());
    pa = new IGBulletParticles(this);
    pv->setParticleAnimator(pa);
    pv->setImage(w->gameManager()->imageNamed("particle"));
    v->addChild(pv);
}

IGBulletManager::~IGBulletManager()
{
    pa->release();
    pv->release();
    v->release();
}

void * IGBulletManager::addBullet(const bullet & b)
{
    bullet_info * i = new bullet_info;
    i->b = b;
    i->body = NULL;
    i->next = bullets;
    i->prev = NULL;
    bullets = i;
    i->body = physicsForBullet(*i);

    return (void*)i;
}

void IGBulletManager::removeBullet(void * h)
{
    bullet_info * i = (bullet_info*)h;
    world->physicsWorld()->DestroyBody(i->body);
    if (i->next)
        i->next->prev = i->prev;
    if (i->prev)
        i->prev->next = i->next;
    if (i==bullets)
        bullets = NULL;
    delete i;
}

b2Body * IGBulletManager::physicsForBullet(bullet_info & b)
{
    b2BodyDef def;
    def.position = b.b.position.b2d();
    def.angle = b.b.rotation;
    def.type = b2_dynamicBody;
    def.bullet = true;
    def.linearVelocity = PHVector2(8, 0).rotated(b.b.rotation).b2d();
    b2Body * bd = world->physicsWorld()->CreateBody(&def);
    b2PolygonShape s;
    s.SetAsBox(0.2, 0.1);
    b2FixtureDef fdef;
    fdef.density = 5.0f;
    fdef.shape = &s;
    fdef.userData = this;
    bd->CreateFixture(&fdef);
    return bd;
}

PHParticleAnimator::particles * IGBulletParticles::calculatedParticles()
{
    int n = 0;
    for (IGBulletManager::bullet_info * p = man->bullets; p; p = p->next)
        switch (p->b.type)
        {
            case 1:
                n++;
                break;
        }
    for (IGBulletManager::bullet_info * p = man->lbullets; p; p = p->next)
        switch (p->b.type)
        {
            case 1:
                n++;
                break;
        }
    part.n = n;
    part.type = particleType2D;
    part.v2d = (particle2D*)realloc(part.v2d, sizeof(particle2D)*n);
    particle2D * v = part.v2d;
    for (IGBulletManager::bullet_info * p = man->bullets; p; p = p->next)
        switch (p->b.type)
        {
            case 1:
                v->color = PHWhiteColor;
                v->position = p->b.position;
                v->rotation = p->b.rotation;
                v->size = PHSize(0.4, 0.2);
                v->ud = (void*)p->b.type;
                v++;
                break;
        }
    for (IGBulletManager::bullet_info * p = man->lbullets; p; p = p->next)
        switch (p->b.type)
        {
            case 1:
                v->color = PHColor(1, 1, 1, p->time);
                v->position = p->b.position;
                v->rotation = p->b.rotation;
                v->size = PHSize(0.4, 0.2);
                v->ud = (void*)p->b.type;
                v++;
                break;
        }
    return &part;
}

PHDrawable * IGBulletManager::loadDrawable()
{
    return (PHDrawable*)v->retain();
}

void IGBulletManager::animate(ph_float elapsed)
{
    for (bullet_info * p = bullets; p; p = p->next)
    {
        p->b.position = p->body->GetPosition();
        p->b.rotation = p->body->GetAngle();
    }
}

//--- Lua Interface ---

static int IGBulletManager_addBullet(lua_State * L)
{
    IGBulletManager * m = (IGBulletManager*)PHLuaThisPointer(L);
    IGBulletManager::bullet b;
    b.type = lua_tonumber(L, 2);
    b.position = PHPoint::fromLua(L, 3);
    b.rotation = lua_tonumber(L, 4);
    b.owner = lua_tonumber(L, 5);
    void * handle = m->addBullet(b); 
    lua_pushlightuserdata(L, handle);
    return 1;
}

static int IGBulletManager_removeBullet(lua_State * L)
{
    IGBulletManager * m = (IGBulletManager*)PHLuaThisPointer(L);
    if (lua_isuserdata(L, 2))
        m->removeBullet(lua_touserdata(L, 2));
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


