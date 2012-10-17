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
    return drawable;
}

void IGObject::animate(ph_float elapsed)
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

//--- Lua Scripting ---

void IGObject::loadLuaInterface(IGScripting * scr)
{
    lua_State * L = scr->luaState();
}
