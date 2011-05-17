//
//  PHLAuxLayer.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/17/11.
//  Copyright 2011 Home. All rights reserved.
//

#include "PHLAuxLayer.h"
#include "PHLua.h"

PHLAuxLayer::PHLAuxLayer()
{
    _class = "PHLAuxLayer";
}

PHLAuxLayer::~PHLAuxLayer()
{
    
}

void PHLAuxLayer::loadFromLua(void * l, const string & root,b2World * world)
{
    PHLObject::loadFromLua(l, root, world);
    int name = -1;
	lua_State * L = (lua_State*)l;
    
    lua_pushstring(L, "name");
    lua_gettable(L, -2);
    if (lua_isnumber(L, -1))
        name = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    
}

void PHLAuxLayer::loadView()
{
    
}