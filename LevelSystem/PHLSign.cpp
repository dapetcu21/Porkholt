//
//  PHLSign.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLSign.h"
#include "PHLua.h"
#include "PHDialog.h"

PHLSign::PHLSign()
{
    _class = "PHLSign";
}

PHLSign::~PHLSign()
{
    
}

void PHLSign::display()
{
    PHDialog * d = new PHDialog;
    d->npc = this;
    d->text = text;
    if (lua_istable(L, 2))
    {
        lua_pushvalue(L, 2);
        d->setCallback(L);
    }
    setDialog(d);
    d->release();
    setShowsQuest(true);
}

void PHLSign::loadFromLua(lua_State * L, const string & root, b2World * world)
{
    PHLNPC::loadFromLua(L,root,world);
    
    lua_getfield(L, -1, "text");
    if (lua_isstring(L,-1))
        text = lua_tostring(L,-1);
    lua_pop(L,1);
}

static int PHLSign_display(lua_State * L)
{
    PHLSign * sgn  = (PHLSign*)PHLuaThisPointer(L);
    sgn->display();
    return 0;
}

void PHLSign::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLSign");
    
    lua_pushcfunction(L, PHLSign_display);
    lua_setfield(L, -2, "_display");
    
    lua_pop(L, 1);
}