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

#include <sstream>

PHLSign::PHLSign()
{
    _class = "PHLSign";
}

PHLSign::~PHLSign()
{
    
}

class PHSignDialog : public PHDialog
{
    void _callback()
    {
        PHLSign * s = (PHLSign*)npc;
        s->advance();
        PHDialog::_callback();
    }
};

void PHLSign::advance()
{
    if (dialogs.empty()) return;
    PHDialog * d = dialogs.front();
    dialogs.pop_front();
    setDialog(d);
    d->release();
}

void PHLSign::display(lua_State * L)
{
    stringstream s(text);
    while (s.good())
    {
        string t;
        getline(s,t,'|');
        PHDialog * d = new PHSignDialog;
        d->npc = this;
        d->text = t;
        dialogs.push_back(d);
    }
    if (L)
        dialogs.back()->setCallback(L);
    advance();
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
    if (lua_istable(L, 2))
    {
        lua_pushvalue(L, 2);
        sgn->display(L);
    } else
        sgn->display();
    return 0;
}

void PHLSign::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLSign");
    
    PHLuaAddMethod_(PHLSign, display);
    
    lua_pop(L, 1);
}