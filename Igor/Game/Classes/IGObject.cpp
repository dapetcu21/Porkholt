/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGObject.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHDrawable.h>

IGObject::IGObject(IGWorld * w) : world(w), L(NULL), scripting(NULL), drawable(NULL)
{
}

void IGObject::attachToWorld(IGWorld * w, bool before, IGObject * ref)
{
    w->insertObject(w, before, ref);
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

    lua_getglobal(L, luaClass());
    lua_getfield(L, -1, "_new");
    lua_pushvalue(L, -1);
    PHLuaCall(L, 1, 1);
    lua_remove(L, -2);
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

void IGObject::initLuaState(IGScripting * scr)
{
    lua_State * L = scr->luaState();
}
