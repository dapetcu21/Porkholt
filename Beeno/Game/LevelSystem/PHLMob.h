/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLMOB_H
#define PHLMOB_H

#include "PHLNPC.h"

class b2Fixture;

class PHLMob: public PHLNPC
{
private:
    ph_float damage;
    
    virtual void contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold);
    
    virtual bool vulnerableFixture(b2Fixture * f);
    virtual ph_float speedNeededForDamagingFixture(b2Fixture * f);
public:
    PHLMob();
    virtual ~PHLMob();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    static void registerLuaInterface(lua_State * L);
    
    void setAttackDamage(ph_float power) { damage = power; }
    ph_float attackDamage() { return damage; }
};

#endif
