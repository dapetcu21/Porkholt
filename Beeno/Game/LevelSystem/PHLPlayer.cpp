
/*
 *  PHLPlayer.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHLPlayer.h"
#include "PHAuxLayerView.h"
#include "PHPlayerView.h"
#include "PHWorld.h"
#include "PHTrailImageView.h"
#include "PHGameManager.h"
#include <Box2D/Box2D.h>
#include "PHMotion.h"
#include "PHLua.h"
#include "PHEventQueue.h"
#include "PHShieldView.h"
#include "PHLevelController.h"
#include "PHTime.h"

PHL_REGISTEROBJECT(PHLPlayer)

PHLPlayer::PHLPlayer() : touchesSomething(false), normal(PHOriginPoint), forceGap(0), mutex(NULL), userInp(true), force(true), damage(1),  _forceGauge(0.0f), maxForce(150), _forceGrowth(100), barHidden(false), shield(false), shieldView(NULL), powerTime(0)
{
	_class = "PHLPlayer";
    maxHP = hp = 3.0f;
}

PHLPlayer::~PHLPlayer()
{
    if (mutex)
        mutex->release();
}

void PHLPlayer::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
	PHLNPC::loadFromLua(L,world,lvlc);
    body->SetBullet(true);
    
    PHLuaGetNumberField(damage, "attackDamage");
    PHLuaGetNumberField(_forceGauge, "forceGauge");
    PHLuaGetNumberField(maxForce, "maximumForce");
    PHLuaGetNumberField(_forceGrowth, "forceGrowth");
#ifndef PH_SIMULATOR
    PHLuaGetBoolField(force, "usesForce");
#endif
    PHLuaGetBoolField(barHidden, "barHidden");
}


#define MAX_TILT (M_PI_2/3)
#define TILT_FORCE_FACTOR 3.0f
#define TOUCH_FORCE_FACTOR 50.0f
void PHLPlayer::updateControls(list<PHPoint> * queue)
{
	if (!body) return;
    ph_float el = gm->frameInterval();
	
	b2Vec2 frc;
	PHTilt t = PHMotion::sharedInstance()->getTilt();
	ph_float f = t.roll;
	if (f>MAX_TILT)
		f = MAX_TILT;
	if (f<-MAX_TILT)
		f = -MAX_TILT;
	frc.y = 0;
	frc.x = -f*TILT_FORCE_FACTOR;
	b2Vec2 center = body->GetWorldCenter();
    if (userInp)
        body->ApplyForce(frc, center);
	ph_float jumpGauge = _forceGauge;
    b2Vec2 totalJump(0,0);
    if (mutex)
        mutex->lock();
    if (!userInp || !force)
        queue->clear();
    bool forceUsed = (!(queue->empty()))&&jumpGauge;
	while (!queue->empty()) {
		frc.x = queue->front().x*TOUCH_FORCE_FACTOR;
		frc.y = queue->front().y*TOUCH_FORCE_FACTOR;
        queue->pop_front();
        if (mutex)
            mutex->unlock();
		ph_float length = sqrt(frc.x*frc.x+frc.y*frc.y);
		if (frc.y<0)
			length = fabs(frc.x);
		if (length)
		{
			if (length>jumpGauge)
			{
				frc.x*=jumpGauge/length;
				if (frc.y>0)
					frc.y*=jumpGauge/length;
				length = jumpGauge;
			}
			jumpGauge -= length;
		}
        frc*=1/60.0f; //leave it 60... the fps doesn't matter
		body->ApplyLinearImpulse(frc, center);
        if (mutex)
            mutex->lock();
	}
    if (mutex)
        mutex->unlock();
    if (forceUsed)
    {
        totalJump.Normalize();
        if (totalJump.y>0.1 && normal.y>0)
        {
            b2Vec2 imp(0,min<ph_float>(normal.y*2,10.0f*el));
            body->ApplyLinearImpulse(imp,center);
        }
    }
    normal.x = normal.y = 0;
    if (touchesSomething>0 && (touchesSomething<1 || !forceGap))
    {
        jumpGauge+=touchesSomething * _forceGrowth * el;
        if (jumpGauge > maxForce)
            jumpGauge = maxForce;
        touchesSomething -= el;
    }
    _forceGauge = jumpGauge;
    if (forceUsed)
        forceGap = 4;
    setUsesTrail(forceGap>0);
    if (forceGap>0)
        forceGap--;
}

void PHLPlayer::updatePosition()
{
    PHLNPC::updatePosition();
    ph_float interval = gm->frameInterval();
    if (powerTime>0)
    {
        powerTime-=interval;
        if (powerTime<=0)
        {
            powerTime = 0;
            deactivatePower();
        }
    }
}

void PHLPlayer::contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse)
{
    PHLObject::contactPostSolve(b, contact, impulse);
    touchesSomething = 1.0f;
    if (contacts.insert(contact).second)
    {
        ph_float a = PHTime::getTime();
        if (a-lastInsert > 1.0f)
        {
            _forceGauge = maxForce;
            lastInsert = a;
        }
    }
    b2Manifold * man = contact->GetManifold();
    b2WorldManifold wMan;
    contact->GetWorldManifold(&wMan);
    int n = man->pointCount;
    b2Vec2 norm = wMan.normal;
    for (int i=0; i<n; i++)
    {
        float val = impulse->normalImpulses[i];
        normal.x+=val*norm.x;
        normal.y+=val*norm.y;
    }
}

void PHLPlayer::contactEnd(bool b,b2Contact* contact)
{
    contacts.erase(contact);
}



void PHLPlayer::_activateShield(PHObject * sender, void * ud)
{
    if (shieldView)
    {
        shieldView->removeFromParent();
        shieldView->release();
    }
    PHRect r = view->bounds();
    if (bodyView)
        r = bodyView->frame();
    shieldView = new PHShieldView(r);
    view->addChild(shieldView);
}

void PHLPlayer::_deactivateShield(PHObject * sender, void * ud)
{
    if (!shieldView) return;
    shieldView->dismiss();
    shieldView->release();
}

void PHLPlayer::deactivateShield()
{
    if (!shield) return;
    shield = false;
    getWorld()->viewEventQueue()->schedule(PHInv(this, PHLPlayer::_deactivateShield,NULL),false);
}

void PHLPlayer::activateShield()
{
    if (shield) return;
    shield = true;
    getWorld()->viewEventQueue()->schedule(PHInv(this, PHLPlayer::_activateShield,NULL),false);
}

void PHLPlayer::updateView(ph_float elapsed, ph_float interpolate)
{
    PHLNPC::updateView(elapsed, interpolate);
    if (powerTime<0)
    {
        PHTrailImageView * iv = dynamic_cast<PHTrailImageView*>(bodyView);
        if (iv)
        {
            if (iv->actualTrailSize() == 0)
            {
                powerTime = 0;
                iv->setAuxImage(NULL);
            }
        }
        else
            powerTime = 0;
    }
}

void PHLPlayer::_activatePower(PHObject * sender, void * ud)
{
    PHTrailImageView * iv = dynamic_cast<PHTrailImageView*>(bodyView);
    if (iv)
        iv->setAuxImage(gm->imageNamed("ball_green"));
    resumeTrail = hasTrail();
    setTrail(true);
}

void PHLPlayer::_deactivatePower(PHObject * sender, void * ud)
{
    setTrail(resumeTrail);
}

void PHLPlayer::activatePower()
{
    ph_float pt = powerTime;
    powerTime = 10.0f;
    if (pt<=0)
    {
        setMaximumForce(maximumForce()*2);
        setForceGrowth(forceGrowth()*2);
        getWorld()->viewEventQueue()->schedule(PHInv(this, PHLPlayer::_activatePower, NULL), false);
    }
}

void PHLPlayer::deactivatePower()
{
    powerTime = -1;
    setMaximumForce(maximumForce()/2);
    setForceGrowth(forceGrowth()/2);
    getWorld()->viewEventQueue()->schedule(PHInv(this, PHLPlayer::_deactivatePower, NULL), false);
}

void PHLPlayer::die()
{
    getWorld()->levelController()->endLevelWithOutcome(PHLevelController::LevelDied);
}

PHLuaBoolGetter(PHLPlayer, userInput);
PHLuaBoolSetter(PHLPlayer, setUserInput);
PHLuaBoolGetter(PHLPlayer, usesForce);
PHLuaBoolSetter(PHLPlayer, setUsesForce);
PHLuaNumberGetter(PHLPlayer, attackDamage);
PHLuaNumberSetter(PHLPlayer, setAttackDamage);
PHLuaNumberGetter(PHLPlayer, forceGauge);
PHLuaNumberSetter(PHLPlayer, setForceGauge);
PHLuaNumberGetter(PHLPlayer, maximumForce);
PHLuaNumberSetter(PHLPlayer, setMaximumForce);
PHLuaNumberGetter(PHLPlayer, forceGrowth);
PHLuaNumberSetter(PHLPlayer, setForceGrowth);
PHLuaBoolGetter(PHLPlayer, isBarHidden);
PHLuaBoolSetter(PHLPlayer, setBarHidden);

void PHLPlayer::registerLuaInterface(lua_State *L)
{
    lua_getglobal(L, "PHLPlayer");
    
    PHLuaAddMethod(PHLPlayer, attackDamage);
    PHLuaAddMethod(PHLPlayer, setAttackDamage);
    PHLuaAddMethod(PHLPlayer, setUserInput);
    PHLuaAddMethod(PHLPlayer, userInput);
    PHLuaAddMethod(PHLPlayer, setUsesForce);
    PHLuaAddMethod(PHLPlayer, usesForce);
    PHLuaAddMethod(PHLPlayer, forceGauge);
    PHLuaAddMethod(PHLPlayer, setForceGauge);
    PHLuaAddMethod(PHLPlayer, maximumForce);
    PHLuaAddMethod(PHLPlayer, setMaximumForce);
    PHLuaAddMethod(PHLPlayer, forceGrowth);
    PHLuaAddMethod(PHLPlayer, setForceGrowth);
    PHLuaAddMethod(PHLPlayer, isBarHidden);
    PHLuaAddMethod(PHLPlayer, setBarHidden);
    
    lua_pop(L, 1);
}
