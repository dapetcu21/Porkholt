
/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLPLAYER_H
#define PHLPLAYER_H

#include "PHLNPC.h"

class PHTrailImageView;
class PHShieldView;

class PHLPlayer : public PHLNPC
{
private:
    PHView * worldView;
    PHPoint normal;
    float touchesSomething;
    int forceGap;
    bool userInp,force;
    float damage;
    PHMutex * mutex;
    
    float powerTime;
    float lastInsert;
    bool resumeTrail;
    set<b2Contact*> contacts;
    
public:
	PHLPlayer();
	virtual ~PHLPlayer();
	
    bool isUsingForce() { return forceGap; }
    
	void updateControls(list<PHPoint> * queue);
	
	virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    virtual void contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse);
    virtual void contactEnd(bool b,b2Contact* contact);
    
    void setMutex(PHMutex * m);
    
    void setUserInput(bool ui) { userInp = ui; }
    bool userInput() { return userInp; }
    void setUsesForce(bool f) { force = f; }
    bool usesForce() { return force; }
    void setAttackDamage( float dmg ) { damage = dmg; }
    float attackDamage() { return damage; }
    
    virtual void updatePosition();
    virtual void updateView(float elapsed, float interpolate);
    
    static void registerLuaInterface(lua_State *L);
private:
    float _forceGauge,maxForce,_forceGrowth;
    bool barHidden;
    bool shield;
    
    PHShieldView * shieldView;
    void _activateShield(PHObject * sender, void * ud);
    void _deactivateShield(PHObject * sender, void * ud);
    void _activatePower(PHObject * sender, void * ud);
    void _deactivatePower(PHObject * sender, void * ud);
    
    virtual void die();
    
public:
    float forceGauge() { return _forceGauge; }
	void setForceGauge(float j) { _forceGauge = j; }
	float maximumForce() { return maxForce; }
	void setMaximumForce(float j) { maxForce = j; }
	float forceGrowth() { return _forceGrowth; }
	void setForceGrowth(float g) { _forceGrowth = g; }
    bool isBarHidden() { return barHidden; } 
    void setBarHidden(bool s) { barHidden = s; }
    
    void activateShield();
    void deactivateShield();
    bool hasShield() { return shield; }
    virtual bool isInvulnerable() { return shield || PHLNPC::isInvulnerable(); }
    
    void activatePower();
    void deactivatePower();
    
 /*   PHObject * retain() { 
        PHLog("%x: retain %d",this,referenceCount()); return PHObject::retain();
    }
    PHObject * release() { 
        PHLog("%x: release %d",this, referenceCount()); return PHObject::release(); 
    } */
};

#endif
