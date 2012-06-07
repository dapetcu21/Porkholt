/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLBOMBERBIRD_H
#define PHLBOMBERBIRD_H

#include "PHLMob.h"

class PHLBomberBird : public PHLMob {
private:
    PHPoint startingPoint;
    PHPoint rotationAxis;
    PHRect lastCameraRect;
    bool attacking;
    bool inCamera;
    bool droppedCargo;
    ph_float accuracy;
    ph_float dropVelocity;
    PHRect bounds;
    
    virtual void updatePosition();
    
public:
    PHLBomberBird();
    virtual ~PHLBomberBird();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    static void registerLuaInterface(lua_State * L);
    
    void attack();
};

#endif
