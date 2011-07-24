//
//  PHLBull.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLBull.h"
#include "PHLua.h"

PHLBull::PHLBull() : PHLMob()
{
    _class = "PHLBull";
}

PHLBull::~PHLBull()
{
    
}

void PHLBull::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L,"PHLBull");
    
    lua_pop(L,1);
}