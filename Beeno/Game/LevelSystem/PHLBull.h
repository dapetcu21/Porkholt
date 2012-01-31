//
//  PHLBull.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLBULL_H
#define PHLBULL_H

#include "PHLMob.h"

class PHLBull : public PHLMob
{
private:
    ph_float attackRange;
    ph_float attackVelocity;
    ph_float attackDuration;
    ph_float cooldownDuration;
    ph_float unrageTime;
    bool attacking;
    virtual void updatePosition();
    void attacked(PHObject * sender, void * ud);
    void cooldownEnded(PHObject * sender, void * ud);
    void reallyAttack(PHObject * sender, void * ud);
    virtual bool vulnerableFixture(b2Fixture * f);
    virtual ph_float speedNeededForDamagingFixture(b2Fixture * f);
public:
    PHLBull();
    virtual ~PHLBull();
    
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    static void registerLuaInterface(lua_State * L);
    
    
    void attack();
};

#endif
