/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLPowerup.h"
#include <Porkholt/Core/PHLua.h>
#include "PHWorld.h"
#include <Porkholt/Core/PHView.h>
#include "PHLPlayer.h"
#include <Porkholt/Core/PHGameManager.h>

PHL_REGISTEROBJECT(PHLPowerup)

PHLPowerup::PHLPowerup() : dismissing(false)
{
    _class = "PHLPowerup";
}

void PHLPowerup::objectEntered(PHLObject * obj)
{
    if (dismissing) return;
    PHWorld *  wrld = getWorld();
    if (obj==wrld->getPlayer())
        collect();
}

void PHLPowerup::collect()
{
    _collected();
    dismiss();
}

void PHLPowerup::collected()
{
    
}

#define animationTime 0.5

void PHLPowerup::dismiss()
{
    if (dismissing) return;
    dismissing = true;
    PHView * v = view->viewWithTag(40);
    if (!v)
    {
        dismissed();
        return;
    }
    remaining = animationTime;
    v->beginCinematicAnimation(animationTime,PHCinematicAnimator::FadeInFunction);
    float scale = 1024;
    v->animateScale(PHSize(1/scale,1/scale));
    v->commitCinematicAnimation();
}

void PHLPowerup::updatePosition()
{
    PHLSensor::updatePosition();
    if (!dismissing) return;
    PHPoint p = getWorld()->getPlayer()->position() - position();
    float elapsed = gm->frameInterval();
    if (elapsed>remaining) elapsed = remaining;
    p*=(animationTime/remaining)*elapsed;
    remaining-=elapsed;
    setPosition(position()+p);
    if (remaining<=0)
    {
        dismissed();
        destroy();
    }
}

void PHLPowerup::dismissed()
{
    if (!L) return;
    PHLuaGetWeakRef(L, this);
    lua_getfield(L, -1, "dismissed");
    lua_pushvalue(L,-2);
    PHLuaCall(L,1,0);
    lua_pop(L,1);
}

void PHLPowerup::_collected()
{
    if (dismissing) return;
    if (L)
    {
        PHLuaGetWeakRef(L, this);
        lua_getfield(L, -1, "collected");
        lua_pushvalue(L,-2);
        PHLuaCall(L,1,0);
        lua_pop(L,1);
    }
    collected();
}

PHLuaDefineCall(PHLPowerup, dismiss);
PHLuaDefineCall(PHLPowerup, collect);

void PHLPowerup::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLPowerup");
    
    PHLuaAddMethod(PHLPowerup, dismiss);
    PHLuaAddMethod(PHLPowerup, collect);
    
    lua_pop(L,1);
}
