//
//  PHLBomberBird.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/8/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
    double accuracy;
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
