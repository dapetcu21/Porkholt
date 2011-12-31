//
//  PHLMob.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLMob.h"
#include "PHLua.h"
#include "PHLPlayer.h"
#include "PHWorld.h"
#include <Box2D/Box2D.h>

PHL_REGISTEROBJECT(PHLMob)

PHLMob::PHLMob() : PHLNPC(), damage(1)
{
    _class = "PHLMob";
}

PHLMob::~PHLMob()
{
    
}

bool PHLMob::vulnerableFixture(b2Fixture * f)
{
    return true;
}

double PHLMob::speedNeededForDamagingFixture(b2Fixture * f)
{
    return 4.0f;
}

void PHLMob::contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold)
{
    PHLObject::contactPreSolve(b, contact, oldManifold);
    if (hinvuln) return;
    PHLPlayer * p = getWorld()->getPlayer();
    if (p->hurt()) return;
    b2Fixture * f1 = contact->GetFixtureA();
    b2Fixture * f2 = contact->GetFixtureB();
    b2Body * bodyA = f1->GetBody();
    b2Body * bodyB = f2->GetBody();
    PHLObject * o1 = (PHLObject*)bodyA->GetUserData();
    PHLObject * o2 = (PHLObject*)bodyB->GetUserData();
    if (o2==this && o1 == p)
    {
        PHLObject * aux = o1;
        o1 = o2;
        o2 = aux;
        b2Fixture * f = f1;
        f1 = f2;
        f2 = f;
    }
    if (o1!=this || o2!=p)
        return;
    
    b2WorldManifold worldManifold;
    contact->GetWorldManifold(&worldManifold);
    b2PointState state1[2], state2[2];
    b2GetPointStates(state1, state2, oldManifold, contact->GetManifold());
    
    if (state2[0] == b2_addState)
        
    {
        
        b2Vec2 point = worldManifold.points[0];
        b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
        b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);
        double approachVelocity = fabs(b2Dot(vB - vA, worldManifold.normal));
        
        if (!isInvulnerable() && (approachVelocity > speedNeededForDamagingFixture(f1) || p->isUsingForce()) && vulnerableFixture(f1))
        {
            decreaseHP(p->attackDamage());
        } else {
            if (!p->isInvulnerable())
            {
                p->decreaseHP(attackDamage());
            } else {
                if (p->hasShield())
                {
                    p->deactivateShield();
                    decreaseHP(p->attackDamage());
                }
            }
        }
    }
}

void PHLMob::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLNPC::loadFromLua(L, world,lvlc);
    PHLuaGetNumberField(damage, "attackDamage");
}

PHLuaNumberGetter(PHLMob, attackDamage);
PHLuaNumberSetter(PHLMob, setAttackDamage);

void PHLMob::registerLuaInterface(lua_State * L)
{   
    lua_getglobal(L,"PHLMob");
    
    PHLuaAddMethod(PHLMob, attackDamage);
    PHLuaAddMethod(PHLMob, setAttackDamage);
    
    lua_pop(L,1);
}