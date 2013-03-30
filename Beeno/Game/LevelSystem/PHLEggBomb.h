/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLEGGBOMB_H
#define PHLEGGBOMB_H

#include "PHLSensor.h"

class PHLEggBomb : public PHLSensor
{
private:
    float magnitude,damage,radius;
    void objectEntered(PHLObject * obj);
public:
    PHLEggBomb();
    virtual ~PHLEggBomb();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    static void registerLuaInterface(lua_State * L);
};

#endif
