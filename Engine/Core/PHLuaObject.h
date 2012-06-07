/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLUAOBJECT_H
#define PHLUAOBJECT_H

#include "PHMain.h"

class PHLuaObject
{
protected:
    string _luaClassName;
    set<lua_State*> _luaStates;
    
private:
    void luaDestroy(PHObject * sender, void * ud);
    
public:
    void luaInstance(lua_State * L, void * this_ud_pointer);
    
    PHLuaObject(const string & className);
    virtual ~PHLuaObject();
};

#endif
