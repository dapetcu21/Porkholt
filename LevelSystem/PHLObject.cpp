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
#include "PHLSensor.h"
#include "PHLGround.h"
#include "PHLSign.h"
#include "PHLBull.h"
#include "PHLMob.h"
#include "PHLLevelEnd.h"
#include "PHLPit.h"
#include "PHLevelController.h"

#include "PHJoint.h"
#include "PHWorld.h"
#include "PHImageView.h"
#include "PHLPowerup.h"
#include "PHLShieldPowerup.h"
#include "PHLHPPowerup.h"
#include "PHLPowerPowerup.h"

#include "PHImage.h"
#include "PHPoofView.h"

#include "PHLua.h"
#include <Box2D/Box2D.h>

template <class T>
PHLObject * newObject() { return (PHLObject*)(new T); }

map<string,PHLObject::initializer> PHLObject::initMap;
bool PHLObject::initialized;

#define addClassForName(class,name) initMap.insert(make_pair<string,initializer>(#name,(initializer)(newObject<class>)))
#define addClass(name) addClassForName(name,name) 

PHLObject * PHLObject::objectWithClass(const string & str)
{
    if (!initialized)
    {
        addClass(PHLObject);
        addClass(PHLCamera);
        addClass(PHLPlayer);
        addClass(PHLAuxLayer);
        addClass(PHLPlatform);
        addClass(PHLSensor);
        addClass(PHLNPC);
        addClass(PHLMob);
        addClass(PHLBull);
        addClass(PHLGround);
        addClass(PHLSign);
        addClass(PHLShieldPowerup);
        addClass(PHLHPPowerup);
        addClass(PHLPowerPowerup);
        addClass(PHLPowerup);
        addClass(PHLLevelEnd);
        addClass(PHLPit);
        
        initialized = true;
    }
    
    map<string,initializer>::iterator i = initMap.find(str);
    if (i==initMap.end())
        return new PHLObject;
    return (i->second)();
}

PHLObject::PHLObject() : _class("PHLObject"), view(NULL), wrld(NULL), world(NULL), body(NULL), rot(0.0f), maxSpeed(FLT_MAX), maxSpeedX(FLT_MAX), maxSpeedY(FLT_MAX), disableLimit(false), hasScripting(false), L(NULL), poofRect(PHNullRect), offset(PHOriginPoint), drfw(false)
{
}

PHLObject::~PHLObject()
{
    scriptingDestroy();
    for (list<PHLAnimation*>::iterator i = animations.begin(); i!=animations.end(); i++)
        if (*i)
            (*i)->release();
	if (wrld&&!drfw)
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
    for (int i=0; i<fixturesDefinitions.size(); i++)
    {
        delete fixturesDefinitions[i]->shape;
        delete fixturesDefinitions[i];
    }
	if (view)
		view->release();
}

void PHLObject::loadBody(void *l)
{
	lua_State * L = (lua_State*)l;
    lua_getfield(L, -1, "physics");
	if (lua_istable(L, -1))
	{
		bool dynamic = false;
        PHLuaGetBoolField(dynamic, "dynamic");
        
        lua_getfield(L,-1, "fixtures");
        if (lua_istable(L, -1))
        {
            int n = 0;
            PHLuaGetNumberField(n, "n");
            if (n)
            {
            
                b2BodyDef def;
                def.position.Set(pos.x,pos.y);
                def.angle = rot;
                def.userData = this;
                def.type = dynamic?b2_dynamicBody:b2_staticBody;
                body = world->CreateBody(&def);
                
                for (int i=0; i<n; i++)
                {
                    lua_pushnumber(L, i);
                    lua_gettable(L, -2);
                    if (lua_istable(L, -1))
                    {
                        
                        //shape attributes
                        const char * typ = "";
                        double circleR=1;
                        PHRect box = PHRect(-0.5f, -0.5f, 1, 1);
                        double rot = 0;
                        PHPoint pos = PHOriginPoint;
                        PHLuaGetStringField(typ,"shape");
                        PHLuaGetRectField(box,"box");
                        PHLuaGetAngleField(rot,"rotation");
                        PHLuaGetNumberField(circleR,"circleR");
                        PHLuaGetPointField(pos,"pos");
                        
                        //physics attributes
                        double friction = 0.3f;
                        double restitution = 0.0f;
                        double density = 1.0f;
                        
                        PHLuaGetNumberField(friction,"friction");
                        PHLuaGetNumberField(restitution,"restitution");
                        PHLuaGetNumberField(density,"density");
                        
                        b2FixtureDef * fdef = new b2FixtureDef;
                        fdef->friction = friction;
                        fdef->density = density;
                        fdef->restitution = restitution;		
                        
                        PHLuaGetNumberField(fdef->filter.groupIndex,"groupIndex");
                        PHLuaGetNumberField(fdef->filter.categoryBits,"categoryBits");
                        PHLuaGetNumberField(fdef->filter.maskBits,"maskBits");
                        
                        if (strcmp(typ, "box")==0)
                        {
                            b2PolygonShape * shape = new b2PolygonShape;
                            b2Vec2 v[4];
                            v[0].Set(box.x,		box.y);
                            v[1].Set(box.x+box.width,	box.y);
                            v[2].Set(box.x+box.width,	box.y+box.height);
                            v[3].Set(box.x,		box.y+box.height);
                            b2Vec2 middle(box.x+box.width/2,box.y+box.height/2);
                            for (int i=0; i<4; i++)
                                b2RotatePoint(v[i],rot,middle);
                            shape->Set(v,4);
                            fdef->shape = shape;
                        }
                        if (strcmp(typ, "circle")==0)
                        {
                            b2CircleShape * shape = new b2CircleShape;
                            shape->m_radius = circleR;
                            shape->m_p.Set(pos.x,pos.y);
                            fdef->shape = shape;					
                        }
                        if (customizeFixture(L,*fdef) && fdef->shape)
                        {
                            fixturesDefinitions.push_back(fdef);
                            fixtures.push_back(NULL);
                        }
                    }
                    lua_pop(L, 1);
                }
                rebuildFixtures();
            }
        }
        lua_pop(L, 1);
	}
	lua_pop(L,1);
}

void PHLObject::rebuildFixtures()
{
    if (!body) return;
    int n = min(fixturesDefinitions.size(),fixtures.size());
    for (int i=0; i<n; i++)
    {
        if (fixtures[i])
            body->DestroyFixture(fixtures[i]);
        fixtures[i] = body->CreateFixture(fixturesDefinitions[i]);
    }
}

bool PHLObject::customizeFixture(lua_State * L, b2FixtureDef & fixtureDef)
{
    return true;
}

void PHLObject::loadFromLua(lua_State * L, b2World * _world, PHLevelController * lvlc)
{
	world = _world;
    const string & root = lvlc->bundlePath();
    
    hasScripting = false;
    lua_getfield(L, -1, "scripting");
    if (lua_isstring(L, -1))
    {
        hasScripting = true;
        scriptingName = lua_tostring(L, -1);
    }
    lua_pop(L,1);
    
    pos = PHOriginPoint;
    PHLuaGetPointField(pos, "pos");
    rot = 0;
    PHLuaGetAngleField(rot, "rotation");
	PHLuaGetNumberField(maxSpeed,"maxVelocity");
    PHLuaGetNumberField(maxSpeedX,"maxVelocityX");
    PHLuaGetNumberField(maxSpeedY,"maxVelocityY");
    PHLuaGetRectField(poofRect, "poofRect");
	
	PHRect min;
	min.x = 0x3f3f3f3f;
	min.y = 0x3f3f3f3f;
	min.height = -0x3f3f3f3f;
	min.width = -0x3f3f3f3f;
	
    lua_getfield(L, -1, "images");
	if (lua_istable(L, -1))
	{
        int n=0;
        PHLuaGetNumberField(n, "n");		
		if (n==0)
			min = PHRect(0, 0, 0, 0);
		
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
            
            Image img;
            PHImageView * image = PHImageView::imageFromLua(L,root,lvlc->animatorPool());    
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
        {
			view->addSubview(i->img);
            i->img->release();
        }
}

void PHLObject::loadView()
{
	view = new PHView(PHRect(viewSize.x+pos.x, viewSize.y+pos.y, viewSize.width, viewSize.height));
    view->setRotationalCenter(PHPoint(-viewSize.x, -viewSize.y));
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
    pos = PHPoint(p.x, p.y);
    rot = body->GetAngle();
}

void PHLObject::updateView()
{
    if (view)
    {
        view->setPosition(pos+viewSize+offset);
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
                    if (a->braking)
                    {
                        b2Vec2 vel = body->GetLinearVelocity();
                        double vlen = vel.Length();
                        vel*=1/vlen;
                        double impulse = vlen*body->GetMass();
                        if (impulse>(a->braking*elapsedTime))
                            impulse=a->braking*elapsedTime;
                        if (impulse)
                        {
                            vel*=-impulse;
                            body->ApplyLinearImpulse(vel,body->GetWorldCenter());
                        }
                    }
                }
                a->animationStepped(elapsedTime);
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

void PHLObject::destroy()
{
    getWorld()->viewEventQueue()->schedule(this, (PHCallback)&PHLObject::_destroy, NULL, false);
}

void PHLObject::_destroy()
{
    getWorld()->removeObject(this);
}

void PHLObject::poof()
{
    getWorld()->viewEventQueue()->schedule(this, (PHCallback)&PHLObject::_poof, NULL, false);
}

void PHLObject::_poof()
{
    if (!poofRect.width || !poofRect.height)
    {
        poofRect = viewSize;
    }
    PHImage * iv = PHPoofView::poofImage();
    double w = iv->width();
    double h = iv->height();
    double dar = w/h;
    double ar = poofRect.width/poofRect.height;
    if (ar>dar)
    {   poofRect.y -= (poofRect.width/dar-poofRect.height)/2;
        poofRect.height = poofRect.width/dar;
    }
    else
    {
        poofRect.x -= (poofRect.height*dar-poofRect.width)/2;
        poofRect.width = poofRect.height*dar;
    }
    PHLNPC * npc = dynamic_cast<PHLNPC*>(this);
    if (npc && npc->isFlipped())
    {
        poofRect.x = -poofRect.x-poofRect.width;
    }
    PHPoofView * v = new PHPoofView(poofRect+position());
    getWorld()->getWorldView()->addSubview(v);
    v->release();
    destroy();
}

#pragma mark -
#pragma mark scripting

void PHLObject::scriptingCreate(lua_State * l)
{
    L = l;
    hasScripting = true;
    lua_getglobal(L, _class.c_str());
    lua_getfield(L, -1, "new");
    lua_pushvalue(L, -2);
    lua_remove(L, -3);
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

PHLuaPointGetter(PHLObject, position);
PHLuaAngleGetter(PHLObject, rotation);
PHLuaPointSetter(PHLObject, setPosition);
PHLuaAngleSetter(PHLObject, setRotation);
PHLuaDefineCall(PHLObject, skipAllAnimations);
PHLuaDefineCall(PHLObject, invalidateAllAnimations);
PHLuaBoolGetter(PHLObject, isDynamic);
PHLuaBoolSetter(PHLObject, setDynamic);
PHLuaPointGetter(PHLObject, velocity);
PHLuaNumberGetter(PHLObject, scalarVelocity);
PHLuaPointSetter(PHLObject, setVelocity);
PHLuaNumberGetter(PHLObject, angularVelocity);
PHLuaNumberSetter(PHLObject, setAngularVelocity);
PHLuaNumberSetter(PHLObject, applyAngularImpulse);
PHLuaNumberGetter(PHLObject, mass);
PHLuaPointGetter(PHLObject, centerOfMass);
PHLuaDefineCall(PHLObject, destroy);
PHLuaDefineCall(PHLObject, poof);

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
        pnt = PHPoint::fromLua(L,2);
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
    PHPoint pnt = PHPoint::fromLua(L,2);
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

static int PHLObject_rotateAround(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);
    obj->rotateAround(-toRad(lua_tonumber(L, 2)), PHPoint::fromLua(L,3));
    return 0;
}

static int PHLObject_worldPoint(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->worldPoint(PHPoint::fromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_localPoint(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->localPoint(PHPoint::fromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_worldVector(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->worldVector(PHPoint::fromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_localVector(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    (obj->localVector(PHPoint::fromLua(L, 2))).saveToLua(L);
    return 1;
}

static int PHLObject_applyImpulse(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    PHPoint ap;
    if (lua_istable(L, 3))
        ap = PHPoint::fromLua(L, 3);
    else
    {
        luaL_argcheck(L, !lua_isnoneornil(L, 3), 3, NULL);
        ap = obj->worldVector(PHOriginPoint);
    }
    obj->applyImpulse(PHPoint::fromLua(L, 2), ap);
    return 0;
}

static int PHLObject_viewWithTag(lua_State * L)
{
    PHLObject * obj = (PHLObject*)PHLuaThisPointer(L);
    luaL_checknumber(L, 2);
    PHView * v = obj->getView()->viewWithTag(lua_tonumber(L, 2));
    if (!v)
        lua_pushnil(L);
    else
        v->getLuaHandle(L);
    return 1;
}

void PHLObject::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLObject");
    
    PHLuaAddMethod(PHLObject, destroy);
    PHLuaAddMethod(PHLObject, poof);
    PHLuaAddMethod(PHLObject, position);
    PHLuaAddMethod(PHLObject, rotation);
    PHLuaAddMethod(PHLObject, transform);
    PHLuaAddMethod(PHLObject, setPosition);
    PHLuaAddMethod(PHLObject, setRotation);
    PHLuaAddMethod(PHLObject, setTransform);
    PHLuaAddMethod(PHLObject, move);
    PHLuaAddMethod(PHLObject, rotate);
    PHLuaAddMethod(PHLObject, rotateAround);
    PHLuaAddMethod(PHLObject, isDynamic);
    PHLuaAddMethod(PHLObject, setDynamic);
    
    PHLuaAddMethod(PHLObject, worldPoint);
    PHLuaAddMethod(PHLObject, localPoint);
    PHLuaAddMethod(PHLObject, worldVector);
    PHLuaAddMethod(PHLObject, localVector);
    PHLuaAddMethod(PHLObject, applyImpulse);
    PHLuaAddMethod(PHLObject, velocity);
    PHLuaAddMethod(PHLObject, scalarVelocity);
    PHLuaAddMethod(PHLObject, setVelocity);
    PHLuaAddMethod(PHLObject, angularVelocity);
    PHLuaAddMethod(PHLObject, setAngularVelocity);
    PHLuaAddMethod(PHLObject, applyAngularImpulse);
    PHLuaAddMethod(PHLObject, mass);
    PHLuaAddMethod(PHLObject, centerOfMass);
    
    PHLuaAddMethod(PHLObject, viewWithTag);
    
    PHLuaAddMethod(PHLObject, invalidateAllAnimations);
    PHLuaAddMethod(PHLObject, skipAllAnimations);
    PHLuaAddMethod(PHLObject, addAnimation);
    
    lua_pop(L, 1);
}

void PHLObject::luaPushSelf(lua_State * l)
{
    if (hasScripting && l==L)
        PHLuaGetWeakRef(L, this);
    else
        lua_pushstring(l, _class.c_str());
}