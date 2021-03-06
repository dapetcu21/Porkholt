/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGStandardBullet.h"
#include "IGScripting.h"
#include "IGBulletManager.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>

IG_REGISTERLUAINTERFACE(IGStandardBullet);

#define b1h 0.1
#define b1w b1h * (128.0/96) * 1.3

IGStandardBullet::IGStandardBullet(IGBulletManager * man, const PHVector2 & _pos, const PHVector2 & vel, int _owner) : IGBullet(man), position(_pos), owner(_owner), contact(NULL)
{
    angle = PHAngleFromVector(vel);

    b2BodyDef bdef;
    bdef.position = _pos.b2d();
    bdef.angle = angle;
    bdef.linearVelocity = vel.b2d();
    bdef.type = b2_dynamicBody;
    bdef.bullet = true;
    bdef.userData = _bulletManager;
    body = _bulletManager->getWorld()->physicsWorld()->CreateBody(&bdef);

    b2PolygonShape shape;
    shape.SetAsBox(b1h/2, b1w/2);

    b2FixtureDef fdef;
    fdef.density = 15.0f;
    fdef.shape = &shape;
    fdef.userData = this;

    if (owner == 1)
    {
        fdef.filter.categoryBits = IGWorld::collisionPlayerBullets;
        fdef.filter.maskBits = IGWorld::collisionAllPlayerBullets;
    } else {
        fdef.filter.categoryBits = IGWorld::collisionMobBullets;
        fdef.filter.maskBits = IGWorld::collisionAllMobBullets;
    }

    body->CreateFixture(&fdef);
}

IGStandardBullet::~IGStandardBullet()
{
    _bulletManager->getWorld()->physicsWorld()->DestroyBody(body);
}

void IGStandardBullet::animate(float elapsed)
{
    position = body->GetPosition();
    angle = body->GetAngle();

    if (contact)
    {
        lua_State * L = _bulletManager->scriptingContext()->luaState();
        PHLuaGetWeakRef(L, _bulletManager);
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "onStandardBulletImpact");
            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, -2);
                PHLuaGetWeakRef(L, contact);
                lua_newtable(L);
                lua_pushlightuserdata(L, this);
                lua_setfield(L, -2, "handle");
                lua_pushnumber(L, owner);
                lua_setfield(L, -2, "owner");
                PHLuaCall(L, 3, 1);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
}

void IGStandardBullet::addParticles(PHParticleAnimator::particle2D * v)
{
    v->position = position;
    v->rotation = angle;
    v->size = PHSize(b1w, b1h);
    v->ud = (void*)(size_t)1;
    v->color = PHWhiteColor;
}

void IGStandardBullet::beginContact(bool aBody, b2Contact * contact)
{
    b2Fixture * f = aBody ? contact->GetFixtureA() : contact->GetFixtureB();
    b2Fixture * of = aBody ? contact->GetFixtureB() : contact->GetFixtureA();
    this->contact = (IGObject*)(of->GetBody()->GetUserData());
}

//--- Lua Interface ---

static int IGBulletManager_newStandardBullet(lua_State * L)
{
    IGBulletManager * man = (IGBulletManager*)PHLuaThisPointer(L, 1);
    PHPoint v(4, 0);
    if (lua_istable(L, 3))
        v = PHPoint::fromLua(L, 3);
    else if (lua_isnumber(L, 3))
        v = PHPoint(lua_isnumber(L, 3), 0);
    int owner = 1;
    if (lua_isnumber(L, 4))
        owner = lua_tonumber(L, 4);
    IGStandardBullet * bullet = new IGStandardBullet(man, PHPoint::fromLua(L, 2), v, owner);
    lua_pushlightuserdata(L, bullet);
    return 1;
}

void IGStandardBullet::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBulletManager");
    PHLuaAddMethod(IGBulletManager, newStandardBullet);
    lua_pop(L, 1);
}
