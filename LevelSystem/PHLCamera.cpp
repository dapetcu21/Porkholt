/*
 *  PHLCamera.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHLCamera.h"
#include "PHLua.h"
#include "PHMainEvents.h"

PHLCamera::PHLCamera() : follow(true), strict(false), strictcount(0), sstrict(0)
{
	_class = "PHLCamera";
}

PHLCamera::~PHLCamera()
{
}

void PHLCamera::loadFromLua(lua_State * L, const string & root,b2World * world)
{
	PHLObject::loadFromLua(L,root,world);
	
	lua_pushstring(L, "camH");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		sz.height = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	PHRect bnd = PHMainEvents::sharedInstance()->screenBounds();
	sz.width = sz.height*bnd.width/bnd.height;
}

void PHLCamera::loadView()
{
}

void PHLCamera::updateCamera(PHPoint pnt)
{
	if (!follow) return;
    
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	
    if (sstrict>0)
    {
        sstrict-=1.0f/fps;
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
	PHLowPassFilter(pos.x, pnt.x, 1.0f/fps, 5.0f);
	PHLowPassFilter(pos.y, pnt.y, 1.0f/fps, 5.0f);
    sz.x = pos.x;
    sz.y = pos.y;
	setPosition(pos);
}

PHLuaBoolGetter(PHLCamera, followsPlayer);
PHLuaBoolSetter(PHLCamera, setFollowsPlayer);

void PHLCamera::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLCamera");
    
    PHLuaAddMethod(PHLCamera, followsPlayer);
    PHLuaAddMethod(PHLCamera, setFollowsPlayer);
    
    lua_pop(L,1);
}