/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGObject.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHDrawable.h>

IGSCRIPTING_REGISTERCLASS("IGObject", IGObject)

IGObject::IGObject(IGWorld * w) : world(w), L(NULL), scripting(NULL), drawable(NULL)
{
}

void IGObject::attachToWorld(IGWorld * w, bool before, IGObject * ref)
{
    w->insertObject(this, before, ref);
}

void IGObject::getLuaObject(IGScripting * s)
{
    if (!scripting)
    {
        scripting = s;
        L = scripting->luaState();
    }

    PHLuaGetWeakRef(L, this);
    if (lua_istable(L, -1))
        return;
    lua_pop(L, 1);

    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setmetatable(L, -2);
    lua_getglobal(L, luaClass());
    lua_setfield(L, -2, "__index");
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ud");

    lua_pushvalue(L, -1);
    PHLuaSetWeakRef(L, this);
}

PHDrawable * IGObject::getDrawable()
{
    if (!drawable)
        drawable = loadDrawable();
    return drawable; }

void IGObject::animate(float elapsed)
{
}

void IGObject::adjustPhysics(float elapsed)
{
}

void IGObject::attachedToWorld()
{
}

PHDrawable * IGObject::loadDrawable()
{
    return NULL;
}

IGObject::~IGObject()
{
    if (scripting)
    {
        PHLuaGetWeakRef(L, this);
        if (lua_istable(L, -1))
        {
            lua_pushnil(L);
            lua_setfield(L, -2, "ud");
        }
        lua_pop(L, 1);
        PHLuaDeleteWeakRef(L, this);
    }
    if (drawable)
        drawable->release();
}

void IGObject::removeFromWorld()
{
    world->removeObject(this);
}

//--- Lua Scripting ---

static int IGObject_attachToWorld(lua_State * L)
{
    IGObject * o = (IGObject*)PHLuaThisPointer(L);
    bool before = true;
    if (lua_isboolean(L, 2))
        before = lua_toboolean(L, 2);
    IGObject * p = NULL;
    if (lua_istable(L, 3))
        p = (IGObject*)PHLuaThisPointer(L, 3);
    o->attachToWorld(o->getWorld(), before, p);
    o->release();
    return 0;
}

static int IGObject_removeFromWorld(lua_State * L)
{
    IGObject * o = (IGObject*)PHLuaThisPointer(L);
    o->removeFromWorld();
    return 0;
}

void IGObject::loadLuaInterface(IGScripting * scr)
{
    lua_State * L = scr->luaState();
    lua_getglobal(L, "IGObject");

    PHLuaAddMethod(IGObject, attachToWorld);
    PHLuaAddMethod(IGObject, removeFromWorld);
    
    lua_pop(L, 1);
}

