/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHSCRIPTABLETIMER_H
#define PHSCRIPTABLETIMER_H

#include <Porkholt/Core/PHTimer.h>
#include <Porkholt/Core/PHLua.h>

struct lua_State;

class PHScriptableTimer : public PHTimer
{
private:
    lua_State * L;
public:
    virtual ~PHScriptableTimer();
    PHScriptableTimer();
    
    lua_State * luaState() { return L; }
    void setLuaState(lua_State * l) { L=l; }
    
    void timerFired();
    static void registerLuaInterface(lua_State * L);
};

#endif
