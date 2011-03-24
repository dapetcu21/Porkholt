/*
 *  PHLCamera.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHLua.h"

PHLCamera::PHLCamera()
{
	_class = "PHLCamera";
}

PHLCamera::~PHLCamera()
{
}

void PHLCamera::loadFromLUA(void * l, const string & root,b2World * world)
{
	PHLObject::loadFromLUA(l,root,world);
	lua_State * L = (lua_State*)l;
	
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
	pnt.x-=sz.width*(1.0f/3-0.5f);
	PHPoint pos = position();
	if (pnt.y<pos.y-sz.height*0.25f)
		pnt.y+=sz.height*0.25f;
	else
	if (pnt.y>pos.y+sz.height*0.25f)
		pnt.y-=sz.height*0.25f;
	else
		pnt.y=pos.y;
	double ox = pos.x;
	PHLowPassFilter(pos.x, pnt.x, 1/60.0f, 5.0f);
	PHLowPassFilter(pos.y, pnt.y, 1/60.0f, 5.0f);
	PHLog("old:%f new:%f delta:%f",ox,pos.x,pos.x-ox);
	setPosition(pos);
}