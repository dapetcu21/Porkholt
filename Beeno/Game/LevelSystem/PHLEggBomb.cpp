//
//  PHLEggBomb.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/8/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLua.h"
#include "PHLEggBomb.h"
#include "PHWorld.h"

PHL_REGISTEROBJECT(PHLEggBomb)

PHLEggBomb::PHLEggBomb() : magnitude(1), damage(1), radius(1)
{
    
}

PHLEggBomb::~PHLEggBomb()
{
    
}

void PHLEggBomb::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLSensor::loadFromLua(L, world, lvlc);
    
    PHLuaGetNumberField(magnitude, "magnitude");
    PHLuaGetNumberField(damage, "damage");
    PHLuaGetNumberField(radius, "radius");
}

void PHLEggBomb::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L,"PHEggBomb");
    
    lua_pop(L,1);
}

void PHLEggBomb::objectEntered(PHLObject * obj)
{
    wrld->boom(position(), magnitude, damage, radius);
    destroy();
}