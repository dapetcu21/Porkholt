/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGBulletManager.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHParticleAnimator.h>
#include <Porkholt/Core/PHLua.h>

#define b1h 0.2
#define b1w 0.2

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
        void advanceAnimation(ph_float elapsed) {}
};

IGBulletManager::IGBulletManager(IGWorld * w) : IGObject(w), bullets(NULL), lbullets(NULL)
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

void * IGBulletManager::addBullet(const bullet & b)
{
    bullet_info * i = new bullet_info;
    i->b = b;
    i->body = NULL;
    i->next = bullets;
    if (i->next)
        i->next->prev = i;
    i->prev = NULL;
    bullets = i;
    i->body = physicsForBullet(*i);
    i->processed = false;
    return (void*)i;
}

void IGBulletManager::removeBullet(void * h)
{
    bullet_info * i = (bullet_info*)h;
    if (i->next)
        i->next->prev = i->prev;
    if (i->prev)
        i->prev->next = i->next;
    if (bullets == i)
        bullets = i->next;
    if (i->b.type == 1)
    {
        i->next = lbullets;
        i->prev = NULL;
        if (i->next)
            i->next->prev = i;
        lbullets = i;
        i->time = 1.0f;
    } else {
        world->physicsWorld()->DestroyBody(i->body);
        delete i;
    }
}

void IGBulletManager::beginContact(bool aBody, b2Contact * contact)
{
    b2Fixture * f = aBody ? contact->GetFixtureA() : contact->GetFixtureB();
    b2Fixture * of = aBody ? contact->GetFixtureB() : contact->GetFixtureA();
    bullet_contact c;
    c.object = (IGObject*)of->GetBody()->GetUserData();

    c.bullet = ((bullet_info*)f->GetUserData());
    contacts.push_back(c);
}

b2Body * IGBulletManager::physicsForBullet(bullet_info & b)
{
    b2BodyDef def;
    def.position = b.b.position.b2d();
    def.angle = b.b.rotation;
    def.type = b2_dynamicBody;
    def.bullet = true;
    def.userData = this;
    def.linearVelocity = PHVector2(4, 0).rotated(b.b.rotation).b2d();
    def.angularVelocity =  -5;
    b2Body * bd = world->physicsWorld()->CreateBody(&def);
    b2CircleShape s;
    s.m_radius = b1w/2;
    b2FixtureDef fdef;
    fdef.density = 15.0f;
    fdef.shape = &s;
    fdef.userData = &b;
    if (b.b.type == 1)
    {
        fdef.filter.categoryBits = IGWorld::collisionPlayerBullets;
        fdef.filter.maskBits = IGWorld::collisionAllPlayerBullets;
    } else {
        fdef.filter.categoryBits = IGWorld::collisionMobBullets;
        fdef.filter.maskBits = IGWorld::collisionAllMobBullets;
    }
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
                v->size = PHSize(b1w, b1h);
                v->ud = (void*)(size_t)p->b.type;
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
                v->size = PHSize(b1w, b1h);
                v->ud = (void*)(size_t)p->b.type;
                v++;
                break;
        }
    return &part;
}

PHDrawable * IGBulletManager::loadDrawable()
{
    return (PHDrawable*)v->retain();
}

bool IGBulletManager::collisionCallback(bullet_info * bullet, IGObject * o)
{
    if (!scripting) return false;
    bool r = false;
    PHLuaGetWeakRef(L, this);
    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "onImpact");
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            PHLuaGetWeakRef(L, o);
            lua_newtable(L);
            lua_pushlightuserdata(L, bullet);
            lua_setfield(L, -2, "handle");
            lua_pushnumber(L, bullet->b.type);
            lua_setfield(L, -2, "type");
            lua_pushnumber(L, bullet->b.owner);
            lua_setfield(L, -2, "owner");
            PHLuaCall(L, 3, 1);
            r = true;
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return r;
}

void IGBulletManager::animate(ph_float elapsed)
{
    for (list<bullet_contact>::iterator i = contacts.begin(); i != contacts.end(); i++)
    {
        if (i->bullet->processed) continue;
        i->bullet->processed = collisionCallback(i->bullet, i->object);
    }
    contacts.clear();
    for (bullet_info * p = bullets; p; p = p->next)
    {
        p->processed = false;
        p->b.position = p->body->GetPosition();
        p->b.rotation = p->body->GetAngle();
    }
    bullet_info * nx;
    for (bullet_info * p = lbullets; p; p = nx)
    {
        nx = p->next;
        p->time -= elapsed*2;
        if (p->time < 0)
        {
            if (p->prev)
                p->prev->next = p->next;
            if (p->next)
                p->next->prev = p->prev;
            if (p == lbullets)
                lbullets = p->next;
            world->physicsWorld()->DestroyBody(p->body);
            delete p;
            continue;
        }
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


