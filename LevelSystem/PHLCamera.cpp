/*
 *  PHLCamera.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
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

void PHLCamera::loadFromLua(void * l, const string & root,b2World * world)
{
	PHLObject::loadFromLua(l,root,world);
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
	
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	
	pnt.x-=sz.width*(1.0f/3-0.5f);
	PHPoint pos = position();
	if (pnt.y<pos.y-sz.height*0.25f)
		pnt.y+=sz.height*0.25f;
	else
	if (pnt.y>pos.y+sz.height*0.25f)
		pnt.y-=sz.height*0.25f;
	else
		pnt.y=pos.y;
	PHLowPassFilter(pos.x, pnt.x, 1.0f/fps, 5.0f);
	PHLowPassFilter(pos.y, pnt.y, 1.0f/fps, 5.0f);
	setPosition(pos);
    
    /*
    PHTilt tilt = PHMotion::sharedInstance()->getTilt();
    double rot = this->rotation();
    double newrot = tilt.roll;
    if (newrot>3)
        newrot=3;
    if (newrot<-3)
        newrot=-3;
    PHLowPassFilter(rot, newrot, 1.0f/fps, 2.0f+1.0f/(fabs(newrot)+1.0f));
    setRotation(rot);
     */
}