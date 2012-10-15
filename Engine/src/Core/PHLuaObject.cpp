/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHLuaObject.h>
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHMessage.h>

PHLuaObject::PHLuaObject(const string & className) : _luaClassName(className)
{
    PHMessage::messageWithName("luaDestroy")->addListener(PHInvN((PHObject*)this, PHLuaObject::luaDestroy));
}


void PHLuaObject::luaDestroy(PHObject * sender, void * ud)
{
    _luaStates.erase((lua_State*)ud);
}

PHLuaObject::~PHLuaObject()
{
    PHMessage::messageWithName("luaDestroy")->removeListener((PHObject*)this);
    for (set<lua_State*>::iterator i=_luaStates.begin(); i!=_luaStates.end(); i++)
    {
        lua_State * L = *i;
        PHLuaGetWeakRef(L, this);
        if (lua_istable(L, -1))
        {
            lua_pushnil(L);
            lua_setfield(L, -2, "ud");
        }
        lua_pop(L, 1);
        PHLuaDeleteWeakRef(L, this);
    }
}

void PHLuaObject::luaInstance(lua_State * L, void * this_ud_pointer = this)
{
    _luaStates.insert(L);
    PHLuaGetWeakRef(L, this);
    if (lua_istable(L, -1))
        return;
    lua_pop(L, 1);
    
    lua_getglobal(L, _luaClassName.c_str());
    lua_getfield(L, -1, "new");
    lua_pushvalue(L, -2);
    PHLuaCall(L, 1, 1);
    lua_remove(L, -2);
    lua_pushlightuserdata(L, this_ud_pointer);
    lua_setfield(L, -2, "ud");
    
    lua_pushvalue(L, -1);
    PHLuaSetWeakRef(L, this);
}
