/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLSensor.h"
#include <Box2D/Box2D.h>
#include <Porkholt/Core/PHLua.h>

PHL_REGISTEROBJECT(PHLSensor)

PHLSensor::PHLSensor() : markAsSensor(true)
{
    _class = "PHLSensor";
}

void PHLSensor::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLuaGetBoolField(markAsSensor, "markAsSensor");
    PHLObject::loadFromLua(L, world, lvlc);
}

bool PHLSensor::customizeFixture(lua_State * L, b2FixtureDef & fixtureDef)
{
    if (markAsSensor)
        fixtureDef.isSensor = true;
    return true;
}

void PHLSensor::_objectEntered(PHLObject * obj)
{
    if (hasScripting)
    {
        PHLuaGetWeakRef(L, this);
        lua_getfield(L, -1, "objectEntered");
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            obj->luaPushSelf(L);
            PHLuaCall(L, 2, 0);
            lua_pop(L, 1);
        } else
            lua_pop(L, 2);
    }
    objectEntered(obj);
}

void PHLSensor::_objectExited(PHLObject * obj)
{
    if (hasScripting)
    {
        PHLuaGetWeakRef(L, this);
        lua_getfield(L, -1, "objectExited");
        if (lua_isfunction(L, -1))
        {
            lua_pushvalue(L, -2);
            obj->luaPushSelf(L);
            PHLuaCall(L, 2, 0);
            lua_pop(L, 1);
        } else
            lua_pop(L, 2);
    }
    objectExited(obj);
}

void PHLSensor::objectEntered(PHLObject * obj)
{
    
}

void PHLSensor::objectExited(PHLObject * obj)
{
    
}

void PHLSensor::contactBegin(bool b,b2Contact* contact)
{
    PHLObject::contactBegin(b, contact);
    PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
    if (o1==this)
        _objectEntered(o2);
    else
    if (o2==this)
        _objectEntered(o1);
}

void PHLSensor::contactEnd(bool b,b2Contact* contact)
{
    PHLObject::contactEnd(b, contact);
    PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
    if (o1==this)
        _objectExited(o2);
    else
    if (o2==this)
        _objectExited(o1);
}
