/*
 *  PHLObject.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHLObject.h"

#include "PHLCamera.h"
#include "PHMainEvents.h"
#include "PHLPlatform.h"
#include "PHLAuxLayer.h"
#include "PHLPlayer.h"
#include "PHEventQueue.h"

#include "PHJoint.h"
#include "PHWorld.h"
#include "PHImageView.h"

#include "PHLua.h"
#include <Box2D/Box2D.h>

PHLObject * PHLObject::objectWithClass(const string & str)
{
	if (str=="PHLCamera")
		return new PHLCamera;
	if (str=="PHLPlayer")
		return new PHLPlayer;
    if (str=="PHLAuxLayer")
        return new PHLAuxLayer;
    if (str=="PHLPlatform")
        return new PHLPlatform;
	return new PHLObject;
}

PHLObject::PHLObject() : _class("PHLObject"), view(NULL), wrld(NULL), world(NULL), body(NULL), rot(0.0f), maxSpeed(FLT_MAX), maxSpeedX(FLT_MAX), maxSpeedY(FLT_MAX), disableLimit(false), hasScripting(false)
{
}

PHLObject::~PHLObject()
{
	if (wrld)
		wrld->removeObject(this);
    list<PHJoint*> jnts = joints;
    joints.clear();
    for (list<PHJoint*>::iterator i = jnts.begin(); i!=jnts.end(); i++)
    {
        PHJoint * joint = *i;
        if (joint->object1()==this)
            joint->setObject1(NULL);
        if (joint->object2()==this)
            joint->setObject2(NULL);
    }
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
		bool isstatic = true;
		lua_pushstring(L, "dynamic");
		lua_gettable(L, -2);
		if (lua_isboolean(L,-1))
			isstatic = !lua_toboolean(L, -1);
		lua_pop(L,1);	
        
        lua_pushstring(L, "fixtures");
        lua_gettable(L, -2);
        if (lua_istable(L, -1))
        {
            int n = 0;
            lua_pushstring(L, "n");
            lua_gettable(L, -2);
            if (lua_isnumber(L, -1))
                n = (int)lua_tonumber(L, -1);
            lua_pop(L, 1);
            
            if (n)
            {
            
                b2BodyDef def;
                def.position.Set(pos.x,pos.y);
                def.angle = -rot*M_PI/180.0f;
                def.userData = this;
                def.type = isstatic?b2_staticBody:b2_dynamicBody;
                body = world->CreateBody(&def);
                
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
                        PHRect box = PHMakeRect(-0.5f, -0.5f, 1, 1);
                        double circleR=1;
                        
                        lua_pushstring(L, "box");
                        lua_gettable(L, -2);
                        if (lua_istable(L, -1))
                            box = PHRect::rectFromLua(L);
                        lua_pop(L,1);
                        
                        double rot = 0;
                        lua_pushstring(L, "rotation");
                        lua_gettable(L, -2);
                        if (lua_isnumber(L, -1))
                            rot = lua_tonumber(L, -1);
                        lua_pop(L,1);
                        
                        lua_pushstring(L, "circleR");
                        lua_gettable(L, -2);
                        if (lua_isnumber(L, -1))
                            circleR = lua_tonumber(L, -1);
                        lua_pop(L,1);
                        
                        PHPoint pos = PHOriginPoint;
                        lua_pushstring(L, "pos");
                        lua_gettable(L, -2);
                        if (lua_istable(L, -1))
                            pos = PHPoint::pointFromLua(L);
                        lua_pop(L, 1);
                        
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
                        
                        b2FixtureDef fdef;
                        fdef.friction = friction;
                        fdef.density = density;
                        fdef.restitution = restitution;				

                        lua_pushstring(L, "groupIndex");
                        lua_gettable(L, -2);
                        if (lua_isnumber(L, -1))
                            fdef.filter.groupIndex = lua_tonumber(L, -1);
                        lua_pop(L,1);
                        
                        lua_pushstring(L, "categoryBits");
                        lua_gettable(L, -2);
                        if (lua_isnumber(L, -1))
                            fdef.filter.categoryBits = lua_tonumber(L, -1);
                        lua_pop(L,1);
                        
                        lua_pushstring(L, "maskBits");
                        lua_gettable(L, -2);
                        if (lua_isnumber(L, -1))
                            fdef.filter.maskBits = lua_tonumber(L, -1);
                        lua_pop(L,1);
                        
                        if (strcmp(typ, "box")==0)
                        {
                            b2PolygonShape shape;
                            b2Vec2 v[4];
                            v[0].Set(box.x,		box.y);
                            v[1].Set(box.x+box.width,	box.y);
                            v[2].Set(box.x+box.width,	box.y+box.height);
                            v[3].Set(box.x,		box.y+box.height);
                            b2Vec2 middle(box.x+box.width/2,box.y+box.height/2);
                            for (int i=0; i<4; i++)
                                b2RotatePoint(v[i],rot,middle);
                            shape.Set(v,4);
                            fdef.shape = &shape;
                            body->CreateFixture(&fdef);
                        }
                        if (strcmp(typ, "circle")==0)
                        {
                            b2CircleShape shape;
                            shape.m_radius = circleR;
                            shape.m_p.Set(pos.x,pos.y);
                            fdef.shape = &shape;
                            body->CreateFixture(&fdef);					
                        }
                    }
                    lua_pop(L, 1);
                }
            }
        }
        lua_pop(L, 1);
	}
	lua_pop(L,1);
}

void PHLObject::loadFromLua(lua_State * L, const string & root, b2World * _world)
{
	world = _world;
    
    hasScripting = false;
    lua_pushstring(L, "scripting");
    lua_gettable(L, -2);
    if (lua_isstring(L, -1))
    {
        hasScripting = true;
        scriptingName = lua_tostring(L, -1);
    }
    lua_pop(L,1);
    
    pos = PHOriginPoint;
	lua_pushstring(L, "pos");
	lua_gettable(L, -2);
	if (lua_istable(L, -1))
		pos = PHPoint::pointFromLua(L);
	lua_pop(L, 1);
	
    rot = 0;
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
            
            Image img;
            PHImageView * image = PHImageView::imageFromLua(L,root);    
            if (image)
            {
                img.img = image;
                img.bounds = image->frame();
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
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	loadBody(L);
	
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
    view->setRotationalCenter(PHMakePoint(-viewSize.x, -viewSize.y));
	loadImages();
	view->setRotation(rot);
}

void PHLObject::setPosition(PHPoint p) 
{
	pos = p; 
	if (view)
		view->setPosition(PHMakePoint(pos.x+viewSize.x, pos.y+viewSize.y));
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),-rot*M_PI/180.0f);
};

void PHLObject::setRotation(double r)
{
	rot = r;
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),-rot*M_PI/180.0f);
}

void PHLObject::setTransform(PHPoint p,double r)
{
    pos = p;
    rot = r;
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),-rot*M_PI/180.0f);
}

void PHLObject::updatePosition()
{
	if (!body)
		return;
	b2Vec2 p = body->GetPosition();
    pos = PHMakePoint(p.x, p.y);
    rot = (-body->GetAngle()*180.0f/M_PI);
}

void PHLObject::updateView()
{
    if (view)
    {
        view->setPosition(PHMakePoint(pos.x+viewSize.x, pos.y+viewSize.y));
        view->setRotation(rot);
    }
}

#define LIMIT_CUTOFF 2.0f

void PHLObject::limitVelocity()
{
	if (!body)
		return;
	if (disableLimit)
	{
		disableLimit = false;
		return;
	}
	
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	double period = 1.0f/fps;
	
	b2Vec2 v = body->GetLinearVelocity();
	double X,Y;
	X=v.x; Y=v.y;
	if (maxSpeed!=FLT_MAX)
	{
		double length = sqrt(X*X+Y*Y);
		if (length>maxSpeed)
			length=maxSpeed/length;
		PHLowPassFilter(X, X*length, period, LIMIT_CUTOFF);
		PHLowPassFilter(Y, Y*length, period, LIMIT_CUTOFF);
	}
	if (maxSpeedX!=FLT_MAX)
	{
		if (X>maxSpeedX)
			PHLowPassFilter(X, maxSpeedX, period, LIMIT_CUTOFF);
		if (X<-maxSpeedX)
			PHLowPassFilter(X, -maxSpeedX, period, LIMIT_CUTOFF);
	}
	if (maxSpeedY!=FLT_MAX)
	{
		if (Y>maxSpeedY)
			PHLowPassFilter(Y, maxSpeedY, period, LIMIT_CUTOFF);
		if (Y<-maxSpeedY)
			PHLowPassFilter(Y, -maxSpeedY, period, LIMIT_CUTOFF);
	}
	v.x = X;
	v.y = Y;
	body->SetLinearVelocity(v);
}

void PHLObject::addJoint(PHJoint * joint)
{
    joints.push_back(joint);
}
void PHLObject::removeJoint(PHJoint * joint)
{
    list<PHJoint*>::iterator i;
    for (i=joints.begin(); i!= joints.end(); i++)
        if (*i==joint)
        {
            joints.erase(i);
            break;
        }
}

bool PHLObject::collidesWith(PHLObject * obj)
{
    return true;
}

void PHLObject::contactBegin(bool b,b2Contact* contact)
{
    
}

void PHLObject::contactEnd(bool b,b2Contact* contact)
{
    
}

void PHLObject::contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void PHLObject::contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

#pragma mark -
#pragma mark scripting

void PHLObject::scriptingCreate(lua_State * L)
{
    hasScripting = true;
    lua_getglobal(L, _class.c_str());
    lua_pushstring(L, "new");
    lua_gettable(L, -2);
    lua_remove(L, -2);
    lua_getglobal(L, _class.c_str());
    lua_pushnil(L);
    lua_pushlightuserdata(L, this);
    int err = lua_pcall(L,3,1,0);
    if (err) {
		PHLog("Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
        lua_pushnil(L);
    } else {
        lua_pushvalue(L, -1);
        PHLuaSetWeakRef(L, this);
    }
}

void PHLObject::scriptingInit(lua_State * L)
{
    if (!hasScripting) return;
    scriptingCreate(L);
    lua_setglobal(L, scriptingName.c_str());
}

static int PHLObject_destroy(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    lua_getglobal(L, "PHWorld");
    lua_getfield(L,-1,"ud");
    PHWorld * world = (PHWorld*)lua_touserdata(L, -1);
    lua_pop(L,2);
    world->removeObject(obj);
    return 0;
}

void PHLObject::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLObject");
    
    lua_pushcfunction(L, PHLObject_destroy);
    lua_setfield(L, -2, "destroy");
    
    lua_pop(L, 1);
}

void PHLObject::defferedLoading(PHWorld * wrld, int insertPos, PHLObject * insObj)
{
    dlipos = insertPos;
    dliobj = insObj;
    dlworld = wrld;
    wrld->viewEventQueue()->schedule(this,(PHCallback)&PHLObject::_defferedLoading, NULL, false);
    retain();
}

void PHLObject::_defferedLoading(PHObject * sender, void * ud)
{
    loadView();
    dlworld->insertObjectAtPosition(this, dlipos, dliobj);
    release();
}