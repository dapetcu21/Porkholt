/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHAutoreleasePool.h>

#ifdef LUA_INTERFACE
#include <Porkholt/Core/PHLua.h>
#endif

void PHObject::bindInvocation(PHInvocation * invo)
{
    set<PHInvocation*> * s = (set<PHInvocation*> *)inv;
    if (!s)
        inv = (void*)(s = new set<PHInvocation*> );
    s->insert(invo);
}

void PHObject::unbindInvocation(PHInvocation * invo)
{
    set<PHInvocation*> * s = (set<PHInvocation*> *)inv;
    if (s)
        s->erase(invo);
}

void PHObject::autorelease()
{
    PHAutoreleasePool::autorelease(this);
}

void PHObject::clearInvocations()
{
    set<PHInvocation*> * s = (set<PHInvocation*> *)inv;
    for (set<PHInvocation*>::iterator i = s->begin(); i!= s->end(); i++)
    {
        (*i)->bound = false;
        (*i)->clear();
    }
    delete s;
}

void PHObject::zombie()
{
    PHLog("HEY YOU! this(%p) is a zombie object. Set a breakpoint in PHObject::zombie() to debug\n", this);
}

#ifdef LUA_INTERFACE
void PHObject::releaseLuaInterface(lua_State * L)
{
    PHLuaDeleteWeakRef(L, this);
    release();
}

PHLuaDefineFunction(PHUDMetaReleaseObject)
{
    PHObject * o = (PHObject*)PHLuaThisPointer(L, 1);
    o->release();
    return 0;
}

void PHObject::pushLuaInstance(lua_State * L, bool strong)
{
    PHLuaGetWeakRef(L, this);
    if (lua_istable(L, -1))
        return;

    lua_newtable(L);

    lua_pushvalue(L, -1);
    lua_setmetatable(L, -2);

    lua_getglobal(L, luaClass().c_str());
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_getglobal(L, "PHObject");
    }
    lua_setfield(L, -2, "__index");

    lua_pushlightuserdata(L, this);
    if (strong) 
    {
        lua_getglobal(L, "PHUDMeta");
        lua_setmetatable(L, -2);
    }
    lua_setfield(L, -2, "ud");

    lua_pushvalue(L, -1);
    PHLuaSetWeakRef(L, this);
    if (strong)
        retain();
}

#include <cxxabi.h>

string PHObject::luaClass()
{
    int status;
    char * s = abi::__cxa_demangle(typeid(*this).name(), 0, 0, &status);
    string r(s);
    free(s);
    return r;
}

PHLuaDefineClass(PHObject, PHLuaBase);

PHLuaDefineMethod(PHObject, className)
{
    lua_pushstring(L, ((PHObject*)PHLuaThisPointer(L, 1))->luaClass().c_str());
    return 1;
}

#endif
