//
//  PHLuaConsole.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 12/7/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLuaConsole.h"
#include "PHLua.h"

PHLuaConsole::PHLuaConsole(lua_State * l) : L(l), PHTCPConsole(1337)
{
    
}

void PHLuaConsole::autoCompleteCLI()
{
    
}

static int PHLuaConsole_PHOutput(lua_State * L)
{
    luaL_checkstring(L, 1);
    string s(lua_tostring(L, 1));
    lua_getglobal(L, "PHOutputConsole");
    if (!lua_isuserdata(L, -1))
        lua_error(L);
    else
    {
        s.append("\r\n");
        ((PHLuaConsole*)lua_touserdata(L, -1))->sendBytes((const uint8_t *)s.c_str(),s.size());
        lua_pop(L,1);
    }
    return 0;
}

void PHLuaConsole::clientConnected()
{
    lua_getglobal(L, "PHOutput");
    lua_setglobal(L, "_PHOutput");
    lua_pushlightuserdata(L, this);
    lua_setglobal(L, "PHOutputConsole");
    lua_pushcfunction(L, PHLuaConsole_PHOutput);
    lua_setglobal(L, "PHOutput");
}

void PHLuaConsole::clientDisconnected()
{
    lua_getglobal(L, "_PHOutput");
    lua_setglobal(L, "PHOutput");
}

bool PHLuaConsole::pharseCLI()
{
    int status = luaL_loadbuffer(L, cli.c_str(), cli.size(), "=console");
    if (status == LUA_ERRSYNTAX)
        return false;
    if (status == LUA_ERRMEM)
    {
        sendBytes((const uint8_t *)"lua: out of memory\r\n", 20);
        return true;
    }
    bool error = (lua_pcall(L, 0, 1, 0)!=0);
    if (lua_isstring(L, -1))
    {
        string ret(error?"lua: ":"");
        ret.append(lua_tostring(L, -1));
        ret.append("\r\n");
        sendBytes((const uint8_t*)ret.c_str(), ret.size());
    }
    return true;
}