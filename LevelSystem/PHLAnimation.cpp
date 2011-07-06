//
//  PHLAnimation.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/4/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLAnimation.h"
#include "PHLua.h"

PHLAnimation::PHLAnimation() : time(0), move(PHOriginPoint),rotateCenter(PHOriginPoint),useRotateCenter(false), objCoord(false), force(PHOriginPoint), impulse(PHOriginPoint), forceapp(PHOriginPoint), velocity(PHOriginPoint), rotate(0), angularImpulse(0), corrForce(INFINITY), elapsed(0), position(0), valid(true), skipped(false), statica(true), odyn(false), L(NULL), function(LinearFunction), next(NULL), cbTarget(NULL), cbFunction(NULL), cbUd(NULL), invalidateCallback(false)
{
    
}

PHLAnimation::~PHLAnimation()
{
    if (next) next->release();
    if (L)
        PHLuaDeleteHardRef(L, this);
}

void PHLAnimation::skipChain()
{
    PHLAnimation * a = this;
    while (a)
    {
        a->skip();
        a=a->nextAnimation();
    };
}

void PHLAnimation::invalidateChain()
{
    PHLAnimation * a = this;
    while (a)
    {
        a->invalidate();
        a=a->nextAnimation();
    };
}

#define bounceL 0.5f

inline double circleEq(double x,double mx,double my,double r)
{
	return sqrt(r*r-(x-mx)*(x-mx))+my;
}

double PHLAnimation::f(double time)
{
    if (function==LinearFunction)
		return time;
    if (function==ConstantFunction)
        return 1.0f;
	if (function==BounceFunction)
	{
		if (time<1-bounceL)
		{
			return time/(1-bounceL);
		} else {
			double m = (bounceL-1);
			double n = 1-m*(1-bounceL);
			double yy = m*(1-bounceL/2)+n;
			double r = sqrt(bounceL*bounceL/4+(1-yy)*(1-yy));
			return circleEq(time, 1-bounceL/2, yy, r);
		}
	}
	if (function==FadeInFunction)
	{
		return time*time;
	}
	if (function==FadeOutFunction)
	{
		return -time*time+2*time;
	}
	if (function==FadeInOutFunction)
	{
		return sin(M_PI*(time-0.5f))/2+0.5;
	}
    if (function==LuaFunction)
    {
        PHLuaGetHardRef(L, this);
        lua_getfield(L, -1, "curveFunction");
        lua_pushnumber(L, time);
        double res = time;
        if (PHLuaCall(L, 1, 1))
        {
            if (lua_isnumber(L, -1))
                res = lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
        lua_pop(L,1);
        return res;
    }
    return time;
}

void PHLAnimation::loadFromLua(lua_State * l)
{
    if (!lua_istable(l, -1))
        return;
    
    L = l;
    
    lua_pushvalue(L, -1);
    PHLuaSetHardRef(L, this);
    
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ud");
    
    lua_getfield(L, -1, "rotation");
    if (lua_isnumber(L, -1))
        rotate = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "movement");
    if (lua_istable(L, -1))
        move = PHPoint::pointFromLua(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "force");
    if (lua_istable(L, -1))
        force = PHPoint::pointFromLua(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "impulse");
    if (lua_istable(L, -1))
        impulse = PHPoint::pointFromLua(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "velocity");
    if (lua_istable(L, -1))
        velocity = PHPoint::pointFromLua(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "correctorForce");
    if (lua_isnumber(L, -1))
        corrForce = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "angularImpulse");
    if (lua_isnumber(L, -1))
        angularImpulse = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "forceApplicationPoint");
    if (lua_istable(L, -1))
        forceapp = PHPoint::pointFromLua(L, -1);
    lua_pop(L,1);
    
    objCoord = false;
    lua_getfield(L, -1, "objectCoordinates");
    if (lua_isboolean(L, -1))
        objCoord = lua_toboolean(L, -1);
    lua_pop(L,1);

    statica = true;
    lua_getfield(L, -1, "disableDynamics");
    if (lua_isboolean(L, -1))
        statica = lua_toboolean(L, -1);
    lua_pop(L,1);
    
    invalidateCallback = false;
    lua_getfield(L, -1, "callbackOnInvalidate");
    if (lua_isboolean(L, -1))
        invalidateCallback = lua_toboolean(L, -1);
    lua_pop(L,1);

    useRotateCenter = false;
    lua_getfield(L, -1, "rotationCenter");
    if (lua_istable(L, -1))
    {
        useRotateCenter = true;
        rotateCenter = PHPoint::pointFromLua(L, -1);
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "nextAnimation");
    if (lua_istable(L, -1))
    {
        PHLAnimation * anim = new PHLAnimation;
        anim->loadFromLua(L);
        setNextAnimation(anim);
        anim->release();
    }
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "time");
    if (lua_isnumber(L, -1))
        time = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    lua_getfield(L, -1, "curveFunction");
    if (lua_isnumber(L, -1))
        function = (int)lua_tonumber(L, -1);
    else
    if (lua_isfunction(L, -1))
        function = LuaFunction;
    else 
        function = LinearFunction;
    if (function<0 || function >=NUMFUNCTIONS)
        function = LinearFunction;
    lua_pop(L, 1);
}

int PHLAnimation_skip(lua_State * L)
{
    PHLAnimation * anim = (PHLAnimation*)PHLuaThisPointer(L);
    anim->skip();
    return 0;
}

int PHLAnimation_skipChain(lua_State * L)
{
    PHLAnimation * anim = (PHLAnimation*)PHLuaThisPointer(L);
    anim->skipChain();
    return 0;
}

int PHLAnimation_invalidate(lua_State * L)
{
    PHLAnimation * anim = (PHLAnimation*)PHLuaThisPointer(L);
    anim->invalidate();
    return 0;
}

int PHLAnimation_invalidateChain(lua_State * L)
{
    PHLAnimation * anim = (PHLAnimation*)PHLuaThisPointer(L);
    anim->invalidateChain();
    return 0;
}

void PHLAnimation::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLAnimation");
    
    lua_pushcfunction(L, PHLAnimation_invalidate);
    lua_setfield(L, -2, "invalidate");
    lua_pushcfunction(L, PHLAnimation_invalidateChain);
    lua_setfield(L, -2, "invalidateChain");
    lua_pushcfunction(L, PHLAnimation_skip);
    lua_setfield(L, -2, "skip");
    lua_pushcfunction(L, PHLAnimation_skipChain);
    lua_setfield(L, -2, "skipChain");
    
    lua_pop(L,1);
}

void PHLAnimation::animationFinished()
{
    if (cbFunction && cbTarget)
        (cbTarget->*cbFunction)(this,cbUd);
    if (L)
    {
        PHLuaGetHardRef(L,this);
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "animationFinished");
            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, -2);
                PHLuaCall(L,1,0);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
}