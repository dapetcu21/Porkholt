/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLAnimation.h"
#include <Porkholt/Core/PHLua.h>

PHLAnimation::PHLAnimation() : time(0), move(PHOriginPoint),rotateCenter(PHOriginPoint),useRotateCenter(false), objCoord(false), force(PHOriginPoint), impulse(PHOriginPoint), forceapp(PHOriginPoint), velocity(PHOriginPoint), rotate(0), angularImpulse(0), corrForce(INFINITY), elapsed(0), position(0), valid(true), skipped(false), statica(true), odyn(false), L(NULL), function(LinearFunction), next(NULL), invalidateCallback(false), braking(0)
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

inline ph_float circleEq(ph_float x,ph_float mx,ph_float my,ph_float r)
{
	return sqrt(r*r-(x-mx)*(x-mx))+my;
}

ph_float PHLAnimation::f(ph_float time)
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
			ph_float m = (bounceL-1);
			ph_float n = 1-m*(1-bounceL);
			ph_float yy = m*(1-bounceL/2)+n;
			ph_float r = sqrt(bounceL*bounceL/4+(1-yy)*(1-yy));
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
        ph_float res = time;
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
    
    PHLuaGetAngleField(rotate, "rotation");
    PHLuaGetPointField(move, "movement");
    PHLuaGetPointField(force, "force");
    PHLuaGetPointField(impulse, "impulse");
    PHLuaGetPointField(velocity, "velocity");
    PHLuaGetPointField(move, "movement");
    PHLuaGetPointField(forceapp, "forceApplicationPoint");
    PHLuaGetNumberField(corrForce, "correctorForce");
    PHLuaGetNumberField(angularImpulse, "angularImpulse");
    PHLuaGetNumberField(braking, "brakeForce");
    objCoord = statica = invalidateCallback = useRotateCenter = false;
    PHLuaGetBoolField(objCoord, "objectCoordinates");
    PHLuaGetBoolField(statica, "disableDynamics");
    PHLuaGetBoolField(invalidateCallback, "callbackOnInvalidate");
    PHLuaGetNumberField(time, "time");
    
    lua_getfield(L, -1, "rotationCenter");
    if (lua_istable(L, -1))
    {
        useRotateCenter = true;
        rotateCenter = PHPoint::fromLua(L, -1);
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

PHLuaDefineCall(PHLAnimation, skip);
PHLuaDefineCall(PHLAnimation, skipChain);
PHLuaDefineCall(PHLAnimation, invalidate);
PHLuaDefineCall(PHLAnimation, invalidateChain);

void PHLAnimation::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLAnimation");
    
    PHLuaAddMethod(PHLAnimation, skip);
    PHLuaAddMethod(PHLAnimation, skipChain);
    PHLuaAddMethod(PHLAnimation, invalidate);
    PHLuaAddMethod(PHLAnimation, invalidateChain);
    
    lua_pop(L,1);
}

void PHLAnimation::animationFinished()
{
    invocation.call(this);
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
