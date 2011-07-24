//
//  PHLMob.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLMob.h"
#include "PHLua.h"

PHLMob::PHLMob() : PHLNPC()
{
    _class = "PHLMob";
}

PHLMob::~PHLMob()
{
    
}

void PHLMob::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L,"PHLMob");
    
    lua_pop(L,1);
}