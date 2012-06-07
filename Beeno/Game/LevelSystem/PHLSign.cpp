/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLSign.h"
#include "PHLua.h"
#include "PHDialog.h"

#include <sstream>

PHL_REGISTEROBJECT(PHLSign)

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

void PHLSign::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLNPC::loadFromLua(L,world,lvlc);
    PHLuaGetStringField(text, "text");
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
