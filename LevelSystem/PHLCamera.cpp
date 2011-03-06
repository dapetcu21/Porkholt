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

void PHLCamera::loadFromLUA(void * l)
{
	PHLObject::loadFromLUA(l);
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