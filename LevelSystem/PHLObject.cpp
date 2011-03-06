/*
 *  PHLObject.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHLua.h"

PHLObject::PHLObject() : _class("PHLObject"), view(NULL), wrld(NULL)
{
	
}

PHLObject::~PHLObject()
{
	if (view)
		view->release();
}

void PHLObject::loadFromLUA(void * l)
{
	lua_State * L = (lua_State*)l;
	lua_pushstring(L, "posX");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		pos.x = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "posY");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		pos.y = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	PHLog("X:%f Y:%f",pos.x,pos.y);
}

void PHLObject::loadView()
{
	view = new PHView(PHMakeRect(0, 0, 0.25, 0.25));
	view->setBackgroundColor(PHWhiteColor); //TMP
	view->setCenter(pos);
}

void PHLObject::setPosition(PHPoint p) 
{
	pos = p; 
	if (view)
		view->setCenter(pos);
};

PHLObject * PHLObject::objectWithClass(const string & str)
{
	if (str=="PHLCamera")
		return new PHLCamera;
	return new PHLObject;
}