//
//  PHScriptableTimer.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/30/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHSCRIPTABLETIMER_H
#define PHSCRIPTABLETIMER_H

#include "PHTimer.h"
#include "PHLua.h"

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