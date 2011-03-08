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

void PHLObject::loadFromLUA(void * l, const string & root)
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
	
	PHRect min;
	min.x = 0x3f3f3f3f;
	min.y = 0x3f3f3f3f;
	min.height = -0x3f3f3f3f;
	min.width = -0x3f3f3f3f;
	
	lua_pushstring(L, "images");
	lua_gettable(L, -2);
	if (lua_istable(L, -1))
	{
		lua_pushstring(L, "n");
		lua_gettable(L, -2);
		int n=0;
		if (lua_isnumber(L, -1))
			n = lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		if (n==0)
			min = PHMakeRect(0, 0, 0, 0);
		
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L, -1))
			{
				bool isV = true;
				Image img;
				string filename = root + "/"; 
				lua_pushstring(L, "filename");
				lua_gettable(L, -2);
				if (isV = lua_isstring(L, -1))
					filename = filename + lua_tostring(L, -1);
				lua_pop(L, 1);
				
				if (isV)
				{
					lua_pushstring(L, "posX");
					lua_gettable(L, -2);
					if (lua_isnumber(L, -1))
						img.bounds.x = lua_tonumber(L, -1);
					lua_pop(L, 1);
					
					lua_pushstring(L, "posY");
					lua_gettable(L, -2);
					if (lua_isnumber(L, -1))
						img.bounds.y = lua_tonumber(L, -1);
					lua_pop(L, 1);
					
					lua_pushstring(L, "imgW");
					lua_gettable(L, -2);
					if (lua_isnumber(L, -1))
						img.bounds.width = lua_tonumber(L, -1);
					lua_pop(L, 1);
					
					lua_pushstring(L, "imgH");
					lua_gettable(L, -2);
					if (lua_isnumber(L, -1))
						img.bounds.height = lua_tonumber(L, -1);
					lua_pop(L, 1);
					
					img.img = new PHImageView();
					img.img->setImage(PHImage::imageFromPath(filename));
					images.push_back(img);
					
					if (img.bounds.x < min.x)
						min.x = img.bounds.x;
					if (img.bounds.y < min.y)
						min.y = img.bounds.y;
					if (img.bounds.x+img.bounds.width > min.width)
						min.width = img.bounds.x+img.bounds.width;
					if (img.bounds.y+img.bounds.height > min.height)
						min.height = img.bounds.y+img.bounds.height;
				}
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	
	min.width -= min.x;
	min.height -= min.y;
	viewSize = min;
	
	for (list<Image>::iterator i = images.begin(); i!=images.end(); i++)
	{
		PHRect b = i->bounds;
		b.x -= viewSize.x;
		b.y -= viewSize.y;
		i->img->setFrame(b);
	}
}

void PHLObject::loadImages()
{
	if (view)
		for (list<Image>::iterator i = images.begin(); i!=images.end(); i++)
			view->addSubview(i->img);
}

void PHLObject::loadView()
{
	view = new PHView(PHMakeRect(viewSize.x+pos.x, viewSize.y+pos.y, viewSize.width, viewSize.height));
	loadImages();
}

void PHLObject::setPosition(PHPoint p) 
{
	pos = p; 
	if (view)
		view->setPosition(PHMakePoint(pos.x+viewSize.x, pos.y+viewSize.y));
};

PHLObject * PHLObject::objectWithClass(const string & str)
{
	if (str=="PHLCamera")
		return new PHLCamera;
	return new PHLObject;
}