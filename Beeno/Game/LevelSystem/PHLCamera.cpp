/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLCamera.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHGameManager.h>

PHL_REGISTEROBJECT(PHLCamera)

PHLCamera::PHLCamera() : follow(true), strict(false), strictcount(0), sstrict(0)
{
	_class = "PHLCamera";
}

PHLCamera::~PHLCamera()
{
}

void PHLCamera::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
	PHLObject::loadFromLua(L,world,lvlc);
    PHLuaGetNumberField(sz.height,"camH");	
	PHRect bnd = gm->screenBounds();
	sz.width = sz.height*bnd.width/bnd.height;
}

void PHLCamera::loadView()
{
}

void PHLCamera::updateCamera(PHPoint pnt, float elapsed)
{
	if (follow)
    {
        
        float el = elapsed;
        
        if (sstrict>0)
        {
            sstrict-=el;
            if (sstrict<=0)
                strict = false;
        }
        
        pnt.x-=sz.width*(1.0f/3-0.5f);
        if (strict)
            pnt.y-=sz.height*(1.0f/4-0.5f);
        else
        {
            PHPoint pos = position();
            if (pnt.y<pos.y-sz.height*0.25f)
                pnt.y+=sz.height*0.25f;
            else
            if (pnt.y>pos.y+sz.height*0.25f)
                pnt.y-=sz.height*0.25f;
            else
                pnt.y=pos.y;
        }
        PHLowPassFilter(&pos.x, pnt.x, el, 5.0f);
        PHLowPassFilter(&pos.y, pnt.y, el, 5.0f);
        setPosition(pos);
    }
    sz.setCenter(position());
}

PHLuaBoolGetter(PHLCamera, followsPlayer);
PHLuaBoolSetter(PHLCamera, setFollowsPlayer);
PHLuaRectGetter(PHLCamera, bounds);

void PHLCamera::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLCamera");
    
    PHLuaAddMethod(PHLCamera, followsPlayer);
    PHLuaAddMethod(PHLCamera, setFollowsPlayer);
    PHLuaAddMethod(PHLCamera, bounds);
    
    lua_pop(L,1);
}
