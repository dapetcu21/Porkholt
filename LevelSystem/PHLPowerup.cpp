//
//  PHLPowerup.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLPowerup.h"
#include "PHLua.h"
#include "PHWorld.h"
#include "PHView.h"
#include "PHLPlayer.h"
#include "PHMainEvents.h"
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
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->view = v;
    double scale = 1024;
    anim->scaleX = 1/scale;
    anim->scaleY = 1/scale;
    anim->time = animationTime;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLPowerup::updatePosition()
{
    PHLSensor::updatePosition();
    if (!dismissing) return;
    PHPoint p = getWorld()->getPlayer()->position() - position();
    double elapsed = 1.0f/(PHMainEvents::sharedInstance()->framesPerSecond());
    if (elapsed>remaining) elapsed = remaining;
    p*=(animationTime/remaining)*elapsed;
    remaining-=elapsed;
    setPosition(position()+p);
    if (remaining<=0)
    {
        dismissed();
        getWorld()->removeObject(this);
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

static int PHLPowerup_dismiss(lua_State * L)
{
    PHLPowerup * pw = (PHLPowerup*)PHLuaThisPointer(L);
    pw->dismiss();
    return 0;
}

static int PHLPowerup_collect(lua_State * L)
{
    PHLPowerup * pw = (PHLPowerup*)PHLuaThisPointer(L);
    pw->collect();
    return 0;
}

void PHLPowerup::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLPowerup");
    
    lua_pushcfunction(L,PHLPowerup_dismiss);
    lua_setfield(L,-2,"dismiss");
    
    lua_pushcfunction(L,PHLPowerup_collect);
    lua_setfield(L,-2,"collect");
    
    lua_pop(L,1);
}