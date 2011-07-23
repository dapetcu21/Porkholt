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
#include "PHMainEvents.h"
#include <Box2D/Box2D.h>
#include "PHMotion.h"
#include "PHLua.h"

PHLPlayer::PHLPlayer() : touchesSomething(false), normal(PHOriginPoint), forceGap(0), mutex(NULL), userInp(true), force(true)
{
	_class = "PHLPlayer";
}

PHLPlayer::~PHLPlayer()
{
    if (mutex)
        mutex->release();
}

void PHLPlayer::loadFromLua(lua_State * L, const string & root,b2World * world)
{
	PHLNPC::loadFromLua(L,root,world);
    body->SetBullet(true);
    
    lua_getfield(L, -1, "usesForce");
    if (lua_isboolean(L,-1))
        force = lua_toboolean(L, -1);
    lua_pop(L,1);
}


#define MAX_TILT 30.0f
#define TILT_FORCE_FACTOR 0.05f
#define TOUCH_FORCE_FACTOR 100.0f
void PHLPlayer::updateControls(list<PHPoint> * queue)
{
	if (!body) return;
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	
	b2Vec2 frc;
	PHTilt t = PHMotion::sharedInstance()->getTilt();
	int f = t.roll;
	if (f>MAX_TILT)
		f = MAX_TILT;
	if (f<-MAX_TILT)
		f = -MAX_TILT;
	frc.y = 0;
	frc.x = f*TILT_FORCE_FACTOR;
	b2Vec2 center = body->GetWorldCenter();
    if (userInp)
        body->ApplyForce(frc, center);
	double jumpGauge = wrld->jumpGauge();
    b2Vec2 totalJump(0,0);
    if (mutex)
        mutex->lock();
    if (!userInp || !force)
        queue->clear();
    bool forceUsed = !(queue->empty());
	while (!queue->empty()) {
		frc.x = queue->front().x*TOUCH_FORCE_FACTOR;
		frc.y = queue->front().y*TOUCH_FORCE_FACTOR;
        queue->pop_front();
        if (mutex)
            mutex->unlock();
		double length = sqrt(frc.x*frc.x+frc.y*frc.y);
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
            b2Vec2 imp(0,min<double>(normal.y*2,10.0f/fps));
            body->ApplyLinearImpulse(imp,center);
        }
    }
    normal.x = normal.y = 0;
    if (touchesSomething>0)
    {
        jumpGauge+=touchesSomething * wrld->jumpGaugeGrowth()/(double)fps;
        double max = wrld->maxJumpGauge();
        if (jumpGauge > max)
            jumpGauge = max;
        touchesSomething -= 1.0f/(double)fps;
    }
	wrld->setJumpGauge(jumpGauge);
    if (forceUsed)
        forceGap = 4;
    setUsesTrail(forceGap>0);
    if (forceGap>0)
        forceGap--;
}

void PHLPlayer::contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse)
{
    touchesSomething = 1.0f;
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

static int PHLPlayer_userInput(lua_State * L)
{
    PHLPlayer * player = (PHLPlayer*)PHLuaThisPointer(L);
    lua_pushboolean(L, player->userInput());
    return 1;
}

static int PHLPlayer_setUserInput(lua_State * L)
{
    PHLPlayer * player = (PHLPlayer*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    player->setUserInput(lua_toboolean(L, 2));
    return 0;
}

static int PHLPlayer_usesForce(lua_State * L)
{
    PHLPlayer * player = (PHLPlayer*)PHLuaThisPointer(L);
    lua_pushboolean(L, player->usesForce());
    return 1;
}

static int PHLPlayer_setUsesForce(lua_State * L)
{
    PHLPlayer * player = (PHLPlayer*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    player->setUsesForce(lua_toboolean(L, 2));
    return 0;
}

void PHLPlayer::registerLuaInterface(lua_State *L)
{
    lua_getglobal(L, "PHLPlayer");
    
    lua_pushcfunction(L, PHLPlayer_setUserInput);
    lua_setfield(L, -2, "setUserInput");
    lua_pushcfunction(L, PHLPlayer_userInput);
    lua_setfield(L, -2, "userInput");
    lua_pushcfunction(L, PHLPlayer_setUsesForce);
    lua_setfield(L, -2, "setUsesForce");
    lua_pushcfunction(L, PHLPlayer_usesForce);
    lua_setfield(L, -2, "usesForce");
    
    lua_pop(L, 1);
}
