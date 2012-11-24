/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGHomingBullet.h"
#include "IGScripting.h"
#include "IGBulletManager.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>

IG_REGISTERLUAINTERFACE(IGHomingBullet);

#define b1h 0.1
#define b1w b1h * (128.0/96) * 1.3

IGHomingBullet::IGHomingBullet(IGBulletManager * man, const PHVector2 & _pos, const PHVector2 & vel, IGObject * _target, int _owner) : IGBullet(man), position(_pos), owner(_owner), contact(NULL), target(_target)
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

IGHomingBullet::~IGHomingBullet()
{
    _bulletManager->getWorld()->physicsWorld()->DestroyBody(body);
}

void IGHomingBullet::animate(ph_float elapsed)
{
    position = body->GetPosition();
    angle = body->GetAngle();

    /*if (target)
    {
        b2Body * b = target->physicsBody();
        PHVector2 p = b->GetPosition();
        ph_float delta = p.y - position.y;
        if (fabs(delta) > 0.1)
        {
            if (delta > 0)
                body->ApplyForceToCenter(b2Vec2(0, 1.5));
            else
                body->ApplyForceToCenter(b2Vec2(0, -1.5));
        }
    }*/

    if (contact)
    {
        lua_State * L = _bulletManager->scriptingContext()->luaState();
        PHLuaGetWeakRef(L, _bulletManager);
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "onHomingBulletImpact");
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

void IGHomingBullet::addParticles(PHParticleAnimator::particle2D * v)
{
    v->position = position;
    v->rotation = angle;
    v->size = PHSize(b1w, b1h);
    v->ud = (void*)(size_t)1;
    v->color = PHWhiteColor;
}

void IGHomingBullet::beginContact(bool aBody, b2Contact * contact)
{
    b2Fixture * f = aBody ? contact->GetFixtureA() : contact->GetFixtureB();
    b2Fixture * of = aBody ? contact->GetFixtureB() : contact->GetFixtureA();
    this->contact = (IGObject*)(of->GetBody()->GetUserData());
}

//--- Lua Interface ---

static int IGBulletManager_newHomingBullet(lua_State * L)
{
    IGBulletManager * man = (IGBulletManager*)PHLuaThisPointer(L, 1);
    PHPoint v(4, 0);
    if (lua_istable(L, 3))
        v = PHPoint::fromLua(L, 3);
    else if (lua_isnumber(L, 3))
        v = PHPoint(lua_isnumber(L, 3), 0);
    IGObject * target = NULL;
    target = (IGObject*)PHLuaThisPointer(L, 4);
    int owner = 1;
    if (lua_isnumber(L, 5))
        owner = lua_tonumber(L, 5);
    IGHomingBullet * bullet = new IGHomingBullet(man, PHPoint::fromLua(L, 2), v, target, owner);
    lua_pushlightuserdata(L, bullet);
    return 1;
}

void IGHomingBullet::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBulletManager");
    PHLuaAddMethod(IGBulletManager, newHomingBullet);
    lua_pop(L, 1);
}
