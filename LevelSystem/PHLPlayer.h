
/*
 *  PHLPlayer.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

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
    double touchesSomething;
    int forceGap;
    bool userInp,force;
    double damage;
    PHMutex * mutex;
    
    double powerTime;
    bool resumeTrail;
    
public:
	PHLPlayer();
	virtual ~PHLPlayer();
	
    bool isUsingForce() { return forceGap; }
    
	void updateControls(list<PHPoint> * queue);
	
	virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    
    virtual void contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse);
    
    void setMutex(PHMutex * m) { if (m) m->retain(); if (mutex) mutex->release(); mutex = m; }
    
    void setUserInput(bool ui) { userInp = ui; }
    bool userInput() { return userInp; }
    void setUsesForce(bool f) { force = f; }
    bool usesForce() { return force; }
    void setAttackDamage( double dmg ) { damage = dmg; }
    double attackDamage() { return damage; }
    
    virtual void updatePosition();
    
    static void registerLuaInterface(lua_State *L);
private:
    double _forceGauge,maxForce,_forceGrowth;
    bool barHidden;
    bool shield;
    
    PHShieldView * shieldView;
    void _activateShield(PHObject * sender, void * ud);
    void _deactivateShield(PHObject * sender, void * ud);
    void _activatePower(PHObject * sender, void * ud);
    void _deactivatePower(PHObject * sender, void * ud);
    
    virtual void die();
    
public:
    double forceGauge() { return _forceGauge; }
	void setForceGauge(double j) { _forceGauge = j; }
	double maximumForce() { return maxForce; }
	void setMaximumForce(double j) { maxForce = j; }
	double forceGrowth() { return _forceGrowth; }
	void setForceGrowth(double g) { _forceGrowth = g; }
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