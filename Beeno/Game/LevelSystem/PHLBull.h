/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLBULL_H
#define PHLBULL_H

#include "PHLMob.h"

class PHLBull : public PHLMob
{
private:
    float attackRange;
    float attackVelocity;
    float attackDuration;
    float cooldownDuration;
    float unrageTime;
    bool attacking;
    virtual void updatePosition();
    void attacked(PHObject * sender, void * ud);
    void cooldownEnded(PHObject * sender, void * ud);
    void reallyAttack(PHObject * sender, void * ud);
    virtual bool vulnerableFixture(b2Fixture * f);
    virtual float speedNeededForDamagingFixture(b2Fixture * f);
public:
    PHLBull();
    virtual ~PHLBull();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    static void registerLuaInterface(lua_State * L);
    
    
    void attack();
};

#endif
