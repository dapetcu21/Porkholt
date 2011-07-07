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
#include "PHLAnimation.h"

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
    scriptingDestroy();
    for (list<PHLAnimation*>::iterator i = animations.begin(); i!=animations.end(); i++)
        if (*i)
            (*i)->release();
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
                def.angle = rot;
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
                            box = PHRect::rectFromLua(L,-1);
                        lua_pop(L,1);
                        
                        double rot = 0;
                        lua_pushstring(L, "rotation");
                        lua_gettable(L, -2);
                        if (lua_isnumber(L, -1))
                            rot = -toRad(lua_tonumber(L, -1));
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
                            pos = PHPoint::pointFromLua(L,-1);
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
		pos = PHPoint::pointFromLua(L,-1);
	lua_pop(L, 1);
	
    rot = 0;
	lua_pushstring(L, "rotation");
	lua_gettable(L, -2);
	if (lua_isnumber(L, -1))
		rot = -toRad(lua_tonumber(L, -1));
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

#pragma mark -
#pragma mark Geometry

void PHLObject::setPosition(PHPoint p) 
{
	pos = p;
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),rot);
};

void PHLObject::setRotation(double r)
{
	rot = r;
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),rot);
}

void PHLObject::setTransform(PHPoint p,double r)
{
    pos = p;
    rot = r;
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),rot);
}

bool PHLObject::isDynamic()
{
    if (!body) return false;
    return (body->GetType()==b2_dynamicBody);
}

void PHLObject::setDynamic(bool d)
{
    if (body)
        body->SetType(d?b2_dynamicBody:b2_staticBody);
}

void PHLObject::rotateAround(double r, PHPoint around)
{
    PHPoint p = pos;
    p-=around;
    p.rotate(r);
    p+=around;
    setTransform(p, rot+r);
}

PHPoint PHLObject::worldPoint(const PHPoint & p)
{
    PHPoint pp(p);
    pp.rotate(rot);
    pp+=pos;
    return pp;
}

PHPoint PHLObject::localPoint(const PHPoint & p)
{
    PHPoint pp(p);
    pp-=pos;
    pp.rotate(-rot);
    return pp;
}

PHPoint PHLObject::worldVector(const PHPoint & p)
{
    return p.rotated(rot);
}

PHPoint PHLObject::localVector(const PHPoint & p)
{
    return p.rotated(-rot);
}

void PHLObject::applyForce(PHPoint force, PHPoint appPoint)
{
    if (body)
        body->ApplyForce(b2Vec2(force.x,force.y),b2Vec2(appPoint.x,appPoint.y));
}

void PHLObject::applyImpulse(PHPoint impulse, PHPoint appPoint)
{
    if (body)
        body->ApplyLinearImpulse(b2Vec2(impulse.x,impulse.y),b2Vec2(appPoint.x,appPoint.y));
}

PHPoint PHLObject::velocity()
{
    if (!body) return PHOriginPoint;
    b2Vec2 v = body->GetLinearVelocity();
    return PHPoint(v.x,v.y);
}

double PHLObject::scalarVelocity()
{
    if (!body) return 0;
    b2Vec2 v = body->GetLinearVelocity();
    return sqrt(v.x*v.x+v.y*v.y);
}

void PHLObject::setVelocity(PHPoint vel)
{
    if (body)
        body->SetLinearVelocity(b2Vec2(vel.x,vel.y));
}

double PHLObject::angularVelocity()
{
    if (!body) return 0;
    return body->GetAngularVelocity();
}

void PHLObject::setAngularVelocity(double v)
{
    if (body)
        body->SetAngularVelocity(v);
}

void PHLObject::applyAngularImpulse(double impulse)
{
    if (body)
        body->ApplyAngularImpulse(impulse);
}

double PHLObject::mass()
{
    if (!body) return 0;
    return body->GetMass();
}

PHPoint PHLObject::centerOfMass()
{
    if (!body) return PHOriginPoint;
    b2Vec2 c = body->GetLocalCenter();
    return PHPoint(c.x,c.y);
}

#pragma mark -
#pragma mark Some other stuff

void PHLObject::updatePosition()
{
	if (!body)
		return;
	b2Vec2 p = body->GetPosition();
    pos = PHMakePoint(p.x, p.y);
    rot = body->GetAngle();
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

#pragma mark -
#pragma mark animations

void PHLObject::addAnimation(PHLAnimation * anim)
{
    if (anim && anim->isValid())
    {
        anim->retain();
        animations.push_back(anim);
        if (anim->statica)
        {
            anim->odyn = isDynamic();
            setDynamic(false);
        }
    }
}

void PHLObject::skipAllAnimations()
{
    for (list<PHLAnimation*>::iterator i = animations.begin(); i!=animations.end(); i++)
        if (*i)
            (*i)->skipChain();
}
void PHLObject::invalidateAllAnimations()
{
    for (list<PHLAnimation*>::iterator i = animations.begin(); i!=animations.end(); i++)
        if (*i)
            (*i)->invalidateChain();
}

void PHLObject::commitAnimations(double el)
{
    list<PHLAnimation*>::iterator i,nx;
    for (i = animations.begin(); i!=animations.end(); i=nx)
    {
        nx = i;
        nx++;
        PHLAnimation * & a = *i;
        bool doneJob = false;
        while (!doneJob && a)
        {
            bool jobFinished = true;
            double tm = a->time;            
            double elapsedTime = el;
            if (a->isValid() && tm && !(tm==INFINITY && a->isSkipped()))
            {
                double remaining = tm-a->elapsed;
                if ((jobFinished=((remaining<=elapsedTime)||a->isSkipped())))
                    elapsedTime = remaining;
                if (!a->isSkipped())
                    doneJob = true;
                a->elapsed +=elapsedTime;
                double opos = a->position;
                double pos = a->f((a->elapsed)/tm);
                a->position = pos;
                double dif = pos-opos;
                PHPoint mv = a->move;
                if (mv.x || mv.y)
                    setPosition(position()+mv*dif);
                if (a->rotate)
                {
                    if (a->useRotateCenter)
                    {
                        PHPoint rc = a->rotateCenter;
                        if (a->objCoord)
                            rc = worldPoint(rc);
                        rotateAround((a->rotate)*dif,rc);
                    }
                    else
                        setRotation(rotation()+(a->rotate)*dif);
                }
                if (body)
                {
                    if (a->angularImpulse)
                        body->ApplyAngularImpulse((a->angularImpulse)*dif);
                    PHPoint frc = a->force;
                    if (frc.x || frc.y)
                    {
                        b2Vec2 force(frc.x*pos,frc.y*pos);
                        if (a->objCoord)
                            force = body->GetWorldVector(force);
                        body->ApplyForce(force,body->GetWorldPoint(b2Vec2(a->forceapp.x,a->forceapp.y)));
                    }
                    frc = a->impulse;
                    if (frc.x || frc.y)
                    {
                        b2Vec2 impulse(frc.x*dif,frc.y*dif);
                        if (a->objCoord)
                            impulse = body->GetWorldVector(impulse);
                        body->ApplyLinearImpulse(impulse,body->GetWorldPoint(b2Vec2(a->forceapp.x,a->forceapp.y)));
                    }
                    frc = a->velocity;
                    if (frc.x || frc.y)
                    {
                        double desired = frc.length();
                        frc/=desired;
                        b2Vec2 vel = body->GetLinearVelocity();
                        double actual = frc.x*vel.x+frc.y*vel.y;
                        double impulse = (desired - actual)*body->GetMass();
                        if (impulse>(a->corrForce*elapsedTime))
                            impulse=a->corrForce*elapsedTime;
                        if (impulse<-(a->corrForce*elapsedTime))
                            impulse=-(a->corrForce*elapsedTime);
                        if (impulse)
                        {
                            frc*=impulse;
                            body->ApplyLinearImpulse(b2Vec2(frc.x,frc.y),body->GetWorldCenter());
                        }
                    }
                }
            }
            if (jobFinished)
            {
                PHLAnimation * olda = a;
                bool dyn = isDynamic();
                if (olda->statica)
                    dyn = olda->odyn;
                a = olda->nextAnimation();
                if (a)
                {
                    a->retain();
                    if (a->statica)
                    {
                        a->odyn = dyn;
                        dyn = false;
                    }
                }
                setDynamic(dyn);
                if (olda->isValid() || olda->invalidateCallback)
                    olda->animationFinished();
                olda->release();
            }
        }
        if (!a)
            animations.erase(i);
    }
}


#pragma mark -
#pragma mark scripting

void PHLObject::scriptingCreate(lua_State * l)
{
    L = l;
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

void PHLObject::scriptingDestroy()
{
    if (L)
        PHLuaDeleteWeakRef(L, this);
}

void PHLObject::scriptingInit(lua_State * l)
{
    if (!hasScripting) return;
    scriptingCreate(l);
    lua_setglobal(l, scriptingName.c_str());
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

static int PHLObject_position(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    obj->position().saveToLua(L);
    return 1;
}

static int PHLObject_rotation(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    lua_pushnumber(L,PHWarp(-toDeg(obj->rotation()),360));
    return 1;
}

static int PHLObject_transform(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    obj->position().saveToLua(L);
    lua_pushnumber(L,PHWarp(-toDeg(obj->rotation()),360));
    return 2;
}

static int PHLObject_setTransform(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    PHPoint pnt;
    double rot;
    bool p = false, r = false;
    if (lua_istable(L, 2))
    {
        pnt = PHPoint::pointFromLua(L,2);
        p =true;
    } else {
        luaL_argcheck(L,!lua_isnoneornil(L,2),2,NULL);
    }
    if (lua_isnumber(L, 3))
    {
        rot = -toRad(lua_tonumber(L, 3));
        r = true;
    } else {
        luaL_argcheck(L,!lua_isnoneornil(L,2),2,NULL);
    }
    if (p&&r)
        obj->setTransform(pnt, rot);
    else
    if (p)
        obj->setPosition(pnt);
    else 
    if (r)
        obj->setRotation(rot);
    return 0;
}

static int PHLObject_setPosition(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    obj->setPosition(PHPoint::pointFromLua(L,2));
    return 0;
}

static int PHLObject_setRotation(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    obj->setRotation(-toRad(lua_tonumber(L, 2)));
    return 0;
}

static int PHLObject_rotate(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    obj->setRotation(obj->rotation()-toRad(lua_tonumber(L, 2)));
    return 0;
}

static int PHLObject_move(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    PHPoint pnt = PHPoint::pointFromLua(L,2);
    PHPoint opnt = obj->position();
    opnt.x +=pnt.x;
    opnt.y +=pnt.y;
    obj->setPosition(opnt);
    return 0;
}

static int PHLObject_addAnimation(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    PHLAnimation * anim = new PHLAnimation;
    lua_pushvalue(L, 2);
    anim->loadFromLua(L);
    lua_pop(L, 1);
    obj->addAnimation(anim);
    return 0;
}

static int PHLObject_skipAllAnimations(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    obj->skipAllAnimations();
    return 0;
}

static int PHLObject_invalidateAllAnimations(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    obj->invalidateAllAnimations();
    return 0;
}

static int PHLObject_rotateAround(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);
    obj->rotateAround(-toRad(lua_tonumber(L, 2)), PHPoint::pointFromLua(L,3));
    return 0;
}

static int PHLObject_isDynamic(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    lua_pushboolean(L, obj->isDynamic());
    return 1;
}

static int PHLObject_setDynamic(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    obj->setDynamic(lua_toboolean(L, 2));
    return 0;
}

static int PHLObject_worldPoint(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->worldPoint(PHPoint::pointFromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_localPoint(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->localPoint(PHPoint::pointFromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_worldVector(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->worldVector(PHPoint::pointFromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_localVector(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->localVector(PHPoint::pointFromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_applyImpulse(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    PHPoint ap;
    if (lua_istable(L, 3))
        ap = PHPoint::pointFromLua(L, 3);
    else
    {
        luaL_argcheck(L, !lua_isnoneornil(L, 3), 3, NULL);
        ap = obj->worldVector(PHOriginPoint);
    }
    obj->applyImpulse(PHPoint::pointFromLua(L, 2), ap);
    return 0;
}

static int PHLObject_velocity(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    obj->velocity().saveToLua(L);
    return 1;
}

static int PHLObject_scalarVelocity(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    lua_pushnumber(L, obj->scalarVelocity());
    return 1;
}

static int PHLObject_setVelocity(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    obj->setVelocity(PHPoint::pointFromLua(L, 2));
    return 0;
}

static int PHLObject_angularVelocity(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    lua_pushnumber(L, obj->angularVelocity());
    return 1;
}

static int PHLObject_setAngularVelocity(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    obj->setAngularVelocity(lua_tonumber(L, 2));
    return 0;
}

static int PHLObject_applyAngularImpulse(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    obj->applyAngularImpulse(lua_tonumber(L, 2));
    return 0;
}

static int PHLObject_mass(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    lua_pushnumber(L, obj->mass());
    return 1;
}

static int PHLObject_centerOfMass(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    obj->centerOfMass().saveToLua(L);
    return 1;
}


void PHLObject::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLObject");
    
    lua_pushcfunction(L, PHLObject_destroy);
    lua_setfield(L, -2, "destroy");
    lua_pushcfunction(L, PHLObject_position);
    lua_setfield(L, -2, "position");
    lua_pushcfunction(L, PHLObject_rotation);
    lua_setfield(L, -2, "rotation");
    lua_pushcfunction(L, PHLObject_transform);
    lua_setfield(L, -2, "transform");
    lua_pushcfunction(L, PHLObject_setPosition);
    lua_setfield(L, -2, "setPosition");
    lua_pushcfunction(L, PHLObject_setRotation);
    lua_setfield(L, -2, "setRotation");
    lua_pushcfunction(L, PHLObject_setTransform);
    lua_setfield(L, -2, "setTransform");
    lua_pushcfunction(L, PHLObject_move);
    lua_setfield(L, -2, "move");
    lua_pushcfunction(L, PHLObject_rotate);
    lua_setfield(L, -2, "rotate");
    lua_pushcfunction(L, PHLObject_rotateAround);
    lua_setfield(L, -2, "rotateAround");
    lua_pushcfunction(L, PHLObject_isDynamic);
    lua_setfield(L, -2, "isDynamic");
    lua_pushcfunction(L, PHLObject_setDynamic);
    lua_setfield(L, -2, "setDynamic");

    lua_pushcfunction(L, PHLObject_worldPoint);
    lua_setfield(L, -2, "worldPoint");
    lua_pushcfunction(L, PHLObject_localPoint);
    lua_setfield(L, -2, "localPoint");
    lua_pushcfunction(L, PHLObject_worldVector);
    lua_setfield(L, -2, "worldVector");
    lua_pushcfunction(L, PHLObject_localVector);
    lua_setfield(L, -2, "localVector");
    lua_pushcfunction(L, PHLObject_applyImpulse);
    lua_setfield(L, -2, "applyImpulse");
    lua_pushcfunction(L, PHLObject_velocity);
    lua_setfield(L, -2, "velocity");
    lua_pushcfunction(L, PHLObject_scalarVelocity);
    lua_setfield(L, -2, "scalarVelocity");
    lua_pushcfunction(L, PHLObject_setVelocity);
    lua_setfield(L, -2, "setVelocity");
    lua_pushcfunction(L, PHLObject_angularVelocity);
    lua_setfield(L, -2, "angularVelocity");
    lua_pushcfunction(L, PHLObject_setAngularVelocity);
    lua_setfield(L, -2, "setAngularVelocity");
    lua_pushcfunction(L, PHLObject_applyAngularImpulse);
    lua_setfield(L, -2, "applyAngularImpulse");
    lua_pushcfunction(L, PHLObject_mass);
    lua_setfield(L, -2, "mass");
    lua_pushcfunction(L, PHLObject_centerOfMass);
    lua_setfield(L, -2, "centerOfMass");
    
    
    lua_pushcfunction(L, PHLObject_invalidateAllAnimations);
    lua_setfield(L, -2, "invalidateAllAnimations");
    lua_pushcfunction(L, PHLObject_rotate);
    lua_setfield(L, -2, "skipAllAnimations");
    lua_pushcfunction(L, PHLObject_addAnimation);
    lua_setfield(L, -2, "addAnimation");
    
    
    lua_pop(L, 1);
}
