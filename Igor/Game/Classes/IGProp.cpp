/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGProp.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHTransformDrawable.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHAnimatorPool.h>

IGSCRIPTING_REGISTERCLASS("IGProp", IGProp)

IGProp::IGProp(IGWorld * world) : IGObject(world), pos(0, 0), rot(0), body(NULL)
{
}

IGProp::~IGProp()
{
}

void IGProp::setPosition(const PHPoint & p)
{
    if (body)
    {
        body->SetTransform(b2Vec2(p.x, p.y), body->GetAngle());
    } else {
        PHTransformDrawable * td = (PHTransformDrawable*)drawable;
        if (td)
            td->setTranslation(p);
    }
    pos = p;
}

void IGProp::setRotation(ph_float r)
{
    if (body)
    {
        body->SetTransform(body->GetPosition(), r);
    } else {
        PHTransformDrawable * td = (PHTransformDrawable*)drawable;
        if (td)
            td->setRotation(PHQuaternion(r, PHVector3(0, 0, 1)));
    }
    rot = r;
}

PHPoint IGProp::worldPoint(const PHPoint & p)
{
    return pos + p.rotated(rot);
}

PHPoint IGProp::localPoint(const PHPoint & p)
{
    return (p - pos).rotated(-rot);
}

void IGProp::setPhysicsBody(b2Body * b)
{
    if (body)
        world->physicsWorld()->DestroyBody(body);
    body = b;
}

PHDrawable * IGProp::loadDrawable()
{
    PHTransformDrawable * td = new PHTransformDrawable;
    td->setTranslation(pos);
    configureDrawable(td);
    return td;
}

void IGProp::animate(ph_float elapsed)
{
    if (body && drawable)
    {
        b2Vec2 v = body->GetPosition();
        float r = body->GetAngle();

        PHTransformDrawable * td = (PHTransformDrawable*)drawable;
        td->setTranslation(PHVector3(v.x, v.y ,0));
        td->setRotation(PHQuaternion(r, PHVector3(0, 0, 1)));

        pos = PHPoint(v);
        rot = r;
    }
}

void IGProp::_beginContact(PHObject * sender, IGObject * o)
{
    if (!L) return;
    PHLuaGetWeakRef(L, this);
    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "beginContact");
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            PHLuaGetWeakRef(L, o);
            if (lua_istable(L, -1))
            {
                PHLuaCall(L, 2, 1);
            } else
                lua_pop(L, 2);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void IGProp::beginContact(bool aBody, b2Contact * c)
{
    IGObject * o = (IGObject*)(aBody ? c->GetFixtureB() : c->GetFixtureA())->GetBody()->GetUserData();
    if (o)
        world->gameManager()->animatorPool()->scheduleAction(PHInvBind(this, IGProp::_beginContact, o));
}

void IGProp::_endContact(PHObject * sender, IGObject * o)
{
    if (!L) return;
    PHLuaGetWeakRef(L, this);
    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "endContact");
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            PHLuaGetWeakRef(L, o);
            if (lua_istable(L, -1))
            {
                PHLuaCall(L, 2, 1);
            } else
                lua_pop(L, 2);
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

void IGProp::endContact(bool aBody, b2Contact * c)
{
    IGObject * o = (IGObject*)(aBody ? c->GetFixtureB() : c->GetFixtureA())->GetBody()->GetUserData();
    if (o)
        world->gameManager()->animatorPool()->scheduleAction(PHInvBind(this, IGProp::_endContact, o));
}

//--- Lua Interface ---

PHLuaPointGetter(IGProp, position)
PHLuaPointSetter(IGProp, setPosition)
PHLuaNumberGetter(IGProp, rotation)
PHLuaNumberSetter(IGProp, setRotation)
PHLuaBoolGetter(IGProp, collisionEvents);
PHLuaBoolSetter(IGProp, setCollisionEvents);

static int IGProp_worldPoint(lua_State * L)
{
    IGProp * o = (IGProp*)PHLuaThisPointer(L);
    o->worldPoint(PHPoint::fromLua(L, 2)).saveToLua(L);
    return 1;
}
static int IGProp_localPoint(lua_State * L)
{
    IGProp * o = (IGProp*)PHLuaThisPointer(L);
    o->localPoint(PHPoint::fromLua(L, 2)).saveToLua(L);
    return 1;
}

static int IGProp_applyLinearImpulse(lua_State * L)
{
    IGProp * o = (IGProp*)PHLuaThisPointer(L);
    b2Body * b = o->physicsBody();
    if (b)
    {
        PHPoint v(0,0), p(b->GetWorldCenter());
        if (lua_istable(L, 2))
            v = PHPoint::fromLua(L, 2);
        if (lua_istable(L, 3))
            p = PHPoint::fromLua(L, 3);
        b->ApplyLinearImpulse(b2Vec2(v.x, v.y), b2Vec2(p.x, p.y));
    }
    return 0;
}

static int IGProp_applyForce(lua_State * L)
{
    IGProp * o = (IGProp*)PHLuaThisPointer(L);
    b2Body * b = o->physicsBody();
    if (b)
    {
        PHPoint v(0,0), p(b->GetWorldCenter());
        if (lua_istable(L, 2))
            v = PHPoint::fromLua(L, 2);
        if (lua_istable(L, 3))
            p = PHPoint::fromLua(L, 3);
        b->ApplyForce(b2Vec2(v.x, v.y), b2Vec2(p.x, p.y));
    }
    return 0;
}

#define numbersetter(m, c) static int IGProp_ ## m (lua_State * L ) { \
    IGProp * o = (IGProp*)PHLuaThisPointer(L); \
    b2Body * b = o->physicsBody(); \
    if (b) \
        b->c(lua_tonumber(L, 2)); \
    return 0; \
}

#define vectorsetter(m, c) static int IGProp_ ## m (lua_State * L ) { \
    IGProp * o = (IGProp*)PHLuaThisPointer(L); \
    b2Body * b = o->physicsBody(); \
    PHPoint p(0, 0); \
    if (lua_istable(L, 2)) \
        p = PHPoint::fromLua(L, 2); \
    if (b) \
        b->c(b2Vec2(p.x, p.y)); \
    return 0; \
}

#define numbergetter(m, c) static int IGProp_ ## m (lua_State * L ) { \
    IGProp * o = (IGProp*)PHLuaThisPointer(L); \
    b2Body * b = o->physicsBody(); \
    if (b) \
        lua_pushnumber(L, b->c()); \
    else \
        lua_pushnil(L); \
    return 1; \
}

#define vectorgetter(m, c) static int IGProp_ ## m (lua_State * L ) { \
    IGProp * o = (IGProp*)PHLuaThisPointer(L); \
    b2Body * b = o->physicsBody(); \
    if (b) \
    { \
        b2Vec2 v = b->c(); \
        PHPoint(v.x, v.y).saveToLua(L); \
    } else \
        lua_pushnil(L); \
    return 1; \
}

#define boolsetter(m, c) static int IGProp_ ## m (lua_State * L ) { \
    IGProp * o = (IGProp*)PHLuaThisPointer(L); \
    b2Body * b = o->physicsBody(); \
    if (b) \
        b->c(lua_toboolean(L, 2)); \
    return 0; \
}

#define boolgetter(m, c) static int IGProp_ ## m (lua_State * L ) { \
    IGProp * o = (IGProp*)PHLuaThisPointer(L); \
    b2Body * b = o->physicsBody(); \
    if (b) \
        lua_pushboolean(L, b->c()); \
    else \
        lua_pushnil(L); \
    return 1; \
}


numbersetter(applyAngularImpulse, ApplyAngularImpulse)
numbersetter(applyTorque, ApplyTorque)
numbersetter(setAngularDamping, SetAngularDamping)
numbersetter(setLinearDamping, SetLinearDamping)
numbersetter(setAngularVelocity, SetAngularVelocity)
vectorsetter(setLinearVelocity, SetLinearVelocity)
numbergetter(angularVelocity, GetAngularVelocity)
vectorgetter(linearVelocity, GetLinearVelocity)
numbergetter(angularDamping, GetAngularDamping)
numbergetter(linearDamping, GetLinearDamping)
boolgetter(isBullet, IsBullet)
boolgetter(isFixedRotation, IsFixedRotation)
boolsetter(setBullet, SetBullet)
boolsetter(setFixedRotation, SetFixedRotation)
numbergetter(mass, GetMass)


void IGProp::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGProp");

    PHLuaAddMethod(IGProp, position);
    PHLuaAddMethod(IGProp, setPosition);
    PHLuaAddMethod(IGProp, rotation);
    PHLuaAddMethod(IGProp, setRotation);

    PHLuaAddMethod(IGProp, worldPoint);
    PHLuaAddMethod(IGProp, localPoint);

    PHLuaAddMethod(IGProp, setAngularVelocity);
    PHLuaAddMethod(IGProp, setLinearVelocity);
    PHLuaAddMethod(IGProp, setAngularDamping);
    PHLuaAddMethod(IGProp, setLinearDamping);

    PHLuaAddMethod(IGProp, angularVelocity);
    PHLuaAddMethod(IGProp, linearVelocity);
    PHLuaAddMethod(IGProp, angularDamping);
    PHLuaAddMethod(IGProp, linearDamping);

    PHLuaAddMethod(IGProp, isBullet);
    PHLuaAddMethod(IGProp, setBullet);
    PHLuaAddMethod(IGProp, isFixedRotation);
    PHLuaAddMethod(IGProp, setFixedRotation);
    PHLuaAddMethod(IGProp, collisionEvents);
    PHLuaAddMethod(IGProp, setCollisionEvents);

    PHLuaAddMethod(IGProp, mass);

    PHLuaAddMethod(IGProp, applyLinearImpulse);
    PHLuaAddMethod(IGProp, applyForce);
    PHLuaAddMethod(IGProp, applyAngularImpulse);
    PHLuaAddMethod(IGProp, applyTorque);

    lua_pop(L, 1);
}

