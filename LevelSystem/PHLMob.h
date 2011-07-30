//
//  PHLMob.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLMOB_H
#define PHLMOB_H

#include "PHLNPC.h"

class b2Fixture;

class PHLMob: public PHLNPC
{
private:
    double damage;
    
    virtual void contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold);
    
    virtual bool vulnerableFixture(b2Fixture * f);
    virtual double speedNeededForDamagingFixture(b2Fixture * f);
public:
    PHLMob();
    virtual ~PHLMob();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    static void registerLuaInterface(lua_State * L);
    
    void setAttackDamage(double power) { damage = power; }
    double attackDamage() { return damage; }
};

#endif
