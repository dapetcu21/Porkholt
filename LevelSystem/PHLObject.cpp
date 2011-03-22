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

PHLObject::PHLObject() : _class("PHLObject"), view(NULL), wrld(NULL), world(NULL), body(NULL), rot(0.0f), maxSpeed(FLT_MAX), maxSpeedX(FLT_MAX), maxSpeedY(FLT_MAX), disableLimit(false)
{
}

PHLObject::~PHLObject()
{
	if (wrld)
		wrld->removeObject(this);
	if (body)
		world->DestroyBody(body);
	if (view)
		view->release();
}

void PHLObject::loadBody(void *l)
{
	lua_State * L = (lua_State*)l;
	lua_pushstring(L, "physics");
	lua_gettable(L, -2);
	if (lua_istable(L, -1))
	{
		lua_pushstring(L, "dynamic");
		lua_gettable(L, -2);
		bool isstatic = lua_isnil(L, -1);
		if (!isstatic && lua_isnumber(L, -1) && lua_tonumber(L, -1)==0)
			isstatic = false;
		lua_pop(L,1);
		
		b2BodyDef def;
		def.position.Set(pos.x,pos.y);
		def.angle = -rot*M_PI/180.0f;
		def.userData = this;
		def.type = isstatic?b2_staticBody:b2_dynamicBody;
		body = world->CreateBody(&def);
		
		int n = 0;
		lua_pushstring(L, "n");
		lua_gettable(L, -2);
		if (lua_isnumber(L, -1))
			n = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L, -1))
			{
				
				//shape attributes
				const char * typ = "";
				lua_pushstring(L, "shape");
				lua_gettable(L, -2);
				if (lua_isstring(L, -1))
					typ = lua_tostring(L,-1);
				lua_pop(L,1);
				double boxH=1, boxW=1, boxX=-0.5f, boxY=-0.5f, circleR=1;
				
				lua_pushstring(L, "boxH");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					boxH = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "boxW");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					boxW = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "boxX");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					boxX = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "boxY");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					boxY = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "circleR");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					circleR = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				//physics attributes
				double friction = 0.3f;
				double restitution = 0.0f;
				double density = 1.0f;
				
				lua_pushstring(L, "friction");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					friction = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "restitution");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					restitution = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "density");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					density = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				lua_pushstring(L, "boxY");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					boxY = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				b2FixtureDef fdef;
				fdef.friction = friction;
				fdef.density = density;
				fdef.restitution = restitution;				

				if (strcmp(typ, "box")==0)
				{
					b2PolygonShape shape;
					b2Vec2 v[4];
					v[0].Set(boxX,		boxY);
					v[1].Set(boxX+boxW,	boxY);
					v[2].Set(boxX+boxW,	boxY+boxH);
					v[3].Set(boxX,		boxY+boxH);
					shape.Set(v,4);
					fdef.shape = &shape;
					body->CreateFixture(&fdef);
				}
				if (strcmp(typ, "circle")==0)
				{
					b2CircleShape shape;
					shape.m_radius = circleR;
					fdef.shape = &shape;
					body->CreateFixture(&fdef);					
				}
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L,1);
}

void PHLObject::loadFromLUA(void * l, const string & root, b2World * _world)
{
	world = _world;
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
	
	lua_pushstring(L, "rotation");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		rot = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "maxVelocity");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		maxSpeed = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "maxVelocityX");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		maxSpeedX = lua_tonumber(L, -1);
	lua_pop(L, 1);
	
	lua_pushstring(L, "maxVelocityY");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		maxSpeedY = lua_tonumber(L, -1);
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
	loadBody(l);
	
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
	view->setRotation(rot);
}

void PHLObject::setPosition(PHPoint p) 
{
	pos = p; 
	if (view)
		view->setPosition(PHMakePoint(pos.x+viewSize.x, pos.y+viewSize.y));
};

void PHLObject::setRotation(double r)
{
	rot = r;
	if (view)
		view->setRotation(rot);
}

void PHLObject::updatePosition()
{
	if (!body)
		return;
	b2Vec2 p = body->GetPosition();
	setPosition(PHMakePoint(p.x, p.y));
	setRotation(-body->GetAngle()*180.0f/M_PI);
}

void PHLObject::limitVelocity()
{
	if (!body)
		return;
	if (disableLimit)
	{
		disableLimit = false;
		return;
	}
	b2Vec2 v = body->GetLinearVelocity();
	if (maxSpeed!=FLT_MAX)
	{
		double length = sqrt(v.x*v.x+v.y*v.y);
		if (length>maxSpeed)
			length=maxSpeed/length;
		v.x*=length;
		v.y*=length;
	}
	if (maxSpeedX!=FLT_MAX)
	{
		if (v.x>maxSpeedX)
			v.x=maxSpeedX;
		if (v.x<-maxSpeedX)
			v.x=-maxSpeedX;
	}
	if (maxSpeedY!=FLT_MAX)
	{
		if (v.y>maxSpeedY)
			v.y=maxSpeedY;
		if (v.y<-maxSpeedY)
			v.y=-maxSpeedY;
	}
	body->SetLinearVelocity(v);
}

PHLObject * PHLObject::objectWithClass(const string & str)
{
	if (str=="PHLCamera")
		return new PHLCamera;
	if (str=="PHLPlayer")
		return new PHLPlayer;
	return new PHLObject;
}