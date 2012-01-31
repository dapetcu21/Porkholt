//
//  PHLEggBomb.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/8/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLEGGBOMB_H
#define PHLEGGBOMB_H

#include "PHLSensor.h"

class PHLEggBomb : public PHLSensor
{
private:
    ph_float magnitude,damage,radius;
    void objectEntered(PHLObject * obj);
public:
    PHLEggBomb();
    virtual ~PHLEggBomb();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    static void registerLuaInterface(lua_State * L);
};

#endif
