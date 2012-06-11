/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLObject.h"

#include <Porkholt/Core/PHGameManager.h>

#include <Porkholt/Core/PHEventQueue.h>
#include "PHLAnimation.h"
#include "PHLevelController.h"
#include <Porkholt/Core/PHKeyframeAnimator.h>

#include "PHJoint.h"
#include "PHWorld.h"
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHBezierPath.h>

#include <Porkholt/Core/PHImage.h>
#include "PHPoofView.h"

#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSoundPool.h>

#include <Porkholt/Core/PHLua.h>
#include <Box2D/Box2D.h>

map<string,PHAllocator> * PHLObject::initMap = NULL;

PHL_REGISTEROBJECT(PHLObject)

PHLObject * PHLObject::objectWithClass(const string & str)
{        
    map<string,PHAllocator>::iterator i = initMap->find(str);
    if (i==initMap->end())
        return new PHLObject;
    return (PHLObject*)(i->second)();
}

PHLObject::PHLObject() : _class("PHLObject"), view(NULL), wrld(NULL), world(NULL), body(NULL), rot(0.0f), maxSpeed(FLT_MAX), maxSpeedX(FLT_MAX), maxSpeedY(FLT_MAX), disableLimit(false), hasScripting(false), L(NULL), poofRect(PHNullRect), offset(PHOriginPoint), drfw(false), gm(NULL), flipped(false), shouldFlipUponLoad(false), patrol(NULL), patSpeed(0.3), patCircle(false), patLength(0), jointLength(0), patPos(0), patRev(0), patSignature(NULL), patP(0), lastPos(0), patLastVel(0,0), patVel(0,0), needsCinePos(false), needsCineRot(false), needsLOmega(false), needsLVel(false), correctRot(false), correctPos(false)
{
}


struct PHBezierFixtureProps
{
    PHRect frame;
    ph_float rotation;
};

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
    for (list<b2FixtureDef*>::iterator i = bezierPaths.begin(); i!=bezierPaths.end(); i++)
    {
        b2FixtureDef * def = *i;
        PHBezierPath * bp = const_cast<PHBezierPath*>((const PHBezierPath*)(def->shape));
        if (bp)
        {
            bp->removeCallback(this);
            bp->release();
        }
        delete (PHBezierFixtureProps*)(def->userData);
        delete def;
    }
    setPatrolPath(NULL);
    for (list<b2FixtureDef*>::iterator i = fixturesDefinitions.begin(); i!=fixturesDefinitions.end(); i++)
    {
        delete (*i)->shape;
        delete (*i);
    }
	if (view)
		view->release();
}

static void b2RotatePoint(b2Vec2 & p, ph_float angle, b2Vec2 around)
{
    p = p-around;
    b2Vec2 pp = p;
    pp.x = cos(angle)*p.x-sin(angle)*p.y;
    pp.y = sin(angle)*p.x+cos(angle)*p.y;
    p = pp;
    p = p+around;
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
                        ph_float circleR=1;
                        PHRect box = PHRect(-0.5f, -0.5f, 1, 1);
                        ph_float rot = 0;
                        PHPoint pos = PHOriginPoint;
                        PHLuaGetStringField(typ,"shape");
                        PHLuaGetRectField(box,"box");
                        PHLuaGetAngleField(rot,"rotation");
                        PHLuaGetNumberField(circleR,"circleR");
                        PHLuaGetPointField(pos,"pos");
                        
                        //physics attributes
                        ph_float friction = 0.3f;
                        ph_float restitution = 0.0f;
                        ph_float density = 1.0f;
                        
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
                        if (strcmp(typ, "freeform")==0)
                        {
                            lua_getfield(L, -1, "curve");
                            PHBezierPath * bp = PHBezierPath::fromLua(L);
                            lua_pop(L,1);
                            if (bp)
                            {
                                bp->addCallback(PHInv(this,PHLObject::bezierCallback,fdef));
                                PHBezierFixtureProps * props = new PHBezierFixtureProps;
                                props->frame = PHWholeRect;
                                PHLuaGetRectField(props->frame, "frame");
                                props->rotation = rot;
                                fdef->userData = props;
                            }
                            fdef->shape = (b2Shape*)bp;
                        }
                        if (customizeFixture(L,*fdef) && fdef->shape)
                        {
                            if (dynamic_cast<const PHBezierPath*>((const PHObject*)(fdef->shape))!=NULL)
                            {
                                bezierPaths.push_back(fdef);
                            } else {
                                fixturesDefinitions.push_back(fdef);
                                fixtures.push_back(NULL);
                            }
                        } else
                            delete fdef;
                    }
                    lua_pop(L, 1);
                }
                rebuildFixtures();
                rebuildBezier(NULL);
            }
        }
        lua_pop(L, 1);
	}
	lua_pop(L,1);
}

void PHLObject::rebuildFixtures()
{
    if (!body) return;
    list<b2FixtureDef*>::iterator i;
    list<b2Fixture*>::iterator j;
    for (i=fixturesDefinitions.begin(),j=fixtures.begin(); (i!=fixturesDefinitions.end()) && (j!=fixtures.end()); i++,j++)
    {
        if (*j)
            body->DestroyFixture(*j);
        (*j) = body->CreateFixture(*i);
    }
}

void PHLObject::bezierCallback(PHObject * sender, void * ud)
{
    b2FixtureDef * def = (b2FixtureDef*)ud;
    rebuildBezier(def);
}

void PHLObject::rebuildBezier(b2FixtureDef * def)
{
    list<b2FixtureDef*>::iterator i,nxi;
    list<b2Fixture*>::iterator j,nxj;
    for (i=nxi=fixturesDefinitions.begin(),j=nxj=fixtures.begin(); (i!=fixturesDefinitions.end()) && (j!=fixtures.end()); i=nxi,j=nxj)
    {
        nxi++;
        nxj++;
        if (def?((def->shape) == (*i)->userData):(dynamic_cast<PHBezierPath*>((PHObject*)((*i)->userData))!=NULL))
        {
            if (*j)
                body->DestroyFixture(*j);
            delete (*i)->shape;
            delete (*i);
            fixturesDefinitions.erase(i);
            fixtures.erase(j);
        }
    }
    if (!def)
        for (list<b2FixtureDef*>::iterator i = bezierPaths.begin(); i!=bezierPaths.end(); i++)
            _buildBezier(*i);
    else
        _buildBezier(def);
}

void PHLObject::_buildBezier(b2FixtureDef * def)
{
    PHBezierPath * bp = const_cast<PHBezierPath*>((const PHBezierPath*)def->shape);
    PHBezierFixtureProps * props = (PHBezierFixtureProps*)def->userData;
    PHRect frm = props->frame;
    PHPoint sz = frm.size();
    PHPoint org = frm.origin();
    PHPoint center = frm.center();
    static const PHPoint half(0.5,0.5);
    const vector<PHBezierPath::anchorPoint> & anchors = bp->calculatedVertices();
    size_t n = 0;
    GLushort * v = PHCurve::triangulatePolygon(anchors, n);
    for (size_t i=0; i<n; i+=3)
    {
        PHPoint p1 =  ((anchors[v[i]].point-half).rotated(props->rotation)+half)*sz+org;
        PHPoint p2 =  ((anchors[v[i+1]].point-half).rotated(props->rotation)+half)*sz+org;
        PHPoint p3 =  ((anchors[v[i+2]].point-half).rotated(props->rotation)+half)*sz+org;
        if (isFlipped())
        {
            PHPoint aux = p1;
            p1 = PHPoint(-p3.x,p3.y);
            p3 = PHPoint(-p1.x,p1.y);
            p2.x = -p2.x;
        }
        const b2Vec2 vertices[3] = { b2Vec2(p1.x,p1.y), b2Vec2(p2.x,p2.y), b2Vec2(p3.x,p3.y) };
        b2PolygonShape * ps = new b2PolygonShape;
        ps->Set(vertices, 3);
        b2FixtureDef * fd = new b2FixtureDef;
        fd->shape = ps;
		fd->userData = bp;
		fd->friction = def->friction;
        fd->restitution = def->restitution;
        fd->density = def->density;
        fd->filter.categoryBits = def->filter.categoryBits;
        fd->filter.maskBits = def->filter.maskBits;
		fd->filter.groupIndex = def->filter.groupIndex;
		fd->isSensor = def->isSensor;
        fixturesDefinitions.push_back(fd);
        fixtures.push_back(body->CreateFixture(fd));
    }
    delete v;    
}

void PHLObject::flip()
{
    flipped = !flipped;
    if (body)
    {
        for (list<b2FixtureDef*>::iterator i = fixturesDefinitions.begin(); i!=fixturesDefinitions.end(); i++)
        {
            b2FixtureDef * d = *i;
            b2Shape * s = const_cast<b2Shape*>(d->shape);
            if (!s) continue;
            if (s->m_type == b2Shape::e_circle)
            {
                b2CircleShape * ss = (b2CircleShape*)s;
                ss->m_p.Set(-ss->m_p.x,ss->m_p.y);
            }
            if (s->m_type == b2Shape::e_polygon)
            {
                b2PolygonShape * ss = (b2PolygonShape*)s;
                int n = ss->GetVertexCount();
                b2Vec2 * v = new b2Vec2[n];
                for (int i=0; i<n; i++)
                {
                    v[i]=ss->GetVertex(n-i-1);
                    v[i].x = -v[i].x;
                }
                ss->Set(v,n);
                delete v;
            }
        }
        rebuildFixtures();
    } 
    view->setHorizontallyFlipped(flipped);
}

bool PHLObject::customizeFixture(lua_State * L, b2FixtureDef & fixtureDef)
{
    return true;
}

void PHLObject::loadFromLua(lua_State * L, b2World * _world, PHLevelController * lvlc)
{
	world = _world;
    const string & root = lvlc->bundlePath();
    gm = lvlc->gameManager();
    
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
    PHLuaGetBoolField(shouldFlipUponLoad,"flipped");
    PHLuaGetBoolField(patCircle, "patrolInCircle");
    PHLuaGetNumberField(patSpeed, "patrolSpeed");
    lua_getfield(L, -1, "patrolPath");
    PHBezierPath * b = PHBezierPath::fromLua(L);
    setPatrolPath(b);
    if (b)
        b->release();
    lua_pop(L, 1);
    PHLuaGetNumberField(patPos,"patrolPosition");
    PHLuaGetBoolField(patRev,"patrolReversed");
    
    
    lua_getfield(L, -1, "keyframeAnimations");
    if (lua_istable(L, -1))
    {
        PHLuaForEach(-1)
            PHKeyframeAnimator * anim = PHKeyframeAnimator::fromLua(L);
            if (anim)
            {
                addCinematicAnimation(anim);
                anim->playSection("default");
                anim->release();
            }
        PHLuaForEach_
    }
    lua_pop(L,1);
    
    
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
            PHImageView * image = PHImageView::imageFromLua(L,gm,root,lvlc->animatorPool());    
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
			view->addChild(i->img);
            i->img->release();
        }
}

void PHLObject::loadView()
{
	view = new PHView(PHRect(viewSize.x+pos.x, viewSize.y+pos.y, viewSize.width, viewSize.height));
    PHPoint center = PHPoint(-viewSize.x, -viewSize.y);
    view->setRotationalCenter(center);
    view->setFlipCenter(center);
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

void PHLObject::setRotation(ph_float r)
{
	rot = r;
    if (body)
        body->SetTransform(b2Vec2(pos.x, pos.y),rot);
}

void PHLObject::setTransform(PHPoint p,ph_float r)
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

void PHLObject::rotateAround(ph_float r, PHPoint around)
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

ph_float PHLObject::scalarVelocity()
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

ph_float PHLObject::angularVelocity()
{
    if (!body) return 0;
    return body->GetAngularVelocity();
}

void PHLObject::setAngularVelocity(ph_float v)
{
    if (body)
        body->SetAngularVelocity(v);
}

void PHLObject::applyAngularImpulse(ph_float impulse)
{
    if (body)
        body->ApplyAngularImpulse(impulse);
}

ph_float PHLObject::mass()
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
#pragma mark Patrol

void PHLObject::setPatrolPath(PHBezierPath *p)
{
    if (p)
        p->retain();
    if (patrol)
        patrol->release();
    patrol = p;
}

void PHLObject::updatePatrol(ph_float elapsed)
{
    if (!patrol)
        return;
    const vector<PHBezierPath::anchorPoint> & v = patrol->calculatedVertices();
    int n = v.size();
    if (!n) return;
    bool rebuild = (&v!=patSignature);
    if (rebuild)
    {
        patSignature = &v;
        patP = 0;
        patLength = 0;
        for (int i=1; i<n; i++)
        {
            PHPoint delta = v[i].point - v[i-1].point;
            ph_float l = delta.length();
            if (patP == i-1 && l<=patLength-lastPos)
            {
                lastPos += l;
                patP = i;
            }
            patLength += l;
        }
        jointLength = (v[0].point - v[n-1].point).length();
    }
    patPos+=(patRev?(-1):1)*patSpeed*elapsed;
    if (patCircle)
    {
        while (patPos<0)
            patPos+=patLength+jointLength;
        while (patPos>=patLength+jointLength)
            patPos-=patLength+jointLength;
    } else {
        if (patPos<0 && !patRev)
            patPos = 0;
        if (patPos>patLength && patRev)
            patPos = patLength;
        while (1)
        {
            if (patPos<=0)
            {
                patPos=-patPos;
                patRev = false;
            }
            if (patPos>=patLength)
            {
                patPos=2*patLength-patPos;
                patRev = true;
            }
            if (patPos>=0 && patPos<=patLength)
                break;
        }
    }
    ph_float delta = patPos - lastPos;
    do {
        int nx = patP+1;
        if (nx>=n)
            nx-=n;
        ph_float d  = (v[nx].point-v[patP].point).length();
        if (d<=delta)
        {
            delta-=d;
            patP = nx;
            lastPos+=d;
        } else
            break;
    } while (1);
    while (delta<0)
    {
        int pv = patP-1;
        if (pv<0)
            pv+=n;
        ph_float d = (v[patP].point-v[pv].point).length();
        delta+=d;
        patP = pv;
        lastPos-=d;
    }
    int nx = patP+1;
    if (nx>=n)
        nx-=n;
    ph_float d  = delta/(v[nx].point-v[patP].point).length();
    PHPoint newPoint = v[patP].point*(1-d)+v[nx].point*d;
    if (body && ! rebuild)
    {
        if (body->GetType() == b2_staticBody)
            body->SetType(b2_kinematicBody);
        b2Vec2 v(body->GetPosition());
        v.x = (newPoint.x - v.x)/elapsed;
        v.y = (newPoint.y - v.y)/elapsed;
        body->SetLinearVelocity(v);
    } else
        setPosition(newPoint);
}

void PHLObject::setCinematicPosition(const PHPoint & pos)
{
    if (body)
    {
        needsCinePos = true;
        cinePos = pos;
    } else
        setPosition(pos);
}

PHPoint PHLObject::cinematicPosition()
{
    if (needsCinePos)
        return cinePos;
    return position();
}

void PHLObject::setCinematicRotation(ph_float rot)
{
    if (body)
    {
        needsCineRot = true;
        cineRot = rot;
    } else
        setRotation(rot);
}

ph_float PHLObject::cinematicRotation()
{
    if (needsCineRot)
        return cineRot;
    return rotation();
}

void PHLObject::updateCinematics(ph_float elapsed)
{
    correctPos=needsCinePos;
    correctRot=needsCineRot;
    if (needsCinePos)
    {
        PHPoint newPoint = cinePos;
        if (body->GetType() == b2_staticBody)
            body->SetType(b2_kinematicBody);
        b2Vec2 v(body->GetPosition());
        v.x = (newPoint.x - v.x)/elapsed;
        v.y = (newPoint.y - v.y)/elapsed;
        if (!needsLVel)
        {
            b2Vec2 lv(body->GetLinearVelocity());
            lastVel.x = lv.x;
            lastVel.y = lv.y;
            needsLVel = true;
        }
        body->SetLinearVelocity(v);
        pos = cinePos;
        needsCinePos = false;
    } else {
        if (needsLVel)
        {
            body->SetLinearVelocity(b2Vec2(lastVel.x,lastVel.y));
            needsLVel = false;
        }
    }
    if (needsCineRot)
    {
        ph_float rot = body->GetAngle();
        if (body->GetType() == b2_staticBody)
            body->SetType(b2_kinematicBody);
        if (!needsLOmega)
        {
            lastOmega = body->GetAngularVelocity();
            needsLOmega = true;
        }
        ph_float delta = cineRot-rot;
        if (abs(delta)>4) //because box2D doesn't behave nicely when abs(delta)>1.5*M_PI
        {
            static const ph_float pi2 = 2*M_PI;
            delta-=(int)(delta/pi2)*pi2;
            if (abs(pi2-delta)<abs(delta))
                delta = pi2-delta;
        }
        body->SetAngularVelocity(delta/elapsed);
        rot = cineRot;
        needsCineRot = false;
    } else {
        if (needsLOmega)
        {
            body->SetAngularVelocity(lastOmega);
            needsLOmega = false;
        }
    }
}

void PHLObject::updatePhysics()
{
    ph_float elapsed = gm->frameInterval();
    PHPoint pp = pos;
    updatePatrol(elapsed);
    updateCinematics(elapsed);
    patLastVel = patVel;
    if (body && patrol)
    {
        b2Vec2 v = body->GetLinearVelocity();
        patVel = PHPoint(v.x,v.y);
        PHPoint delta = patVel-patLastVel;
        if (!delta.x && !delta.y) return;
        b2ContactEdge * cl = body->GetContactList();
        while (cl)
        {
            b2Body * b = cl->other;
            if (b)
            {
                b2Vec2 v(b->GetLinearVelocity());
                v.x+=delta.x;
                v.y+=delta.y;
                b->SetLinearVelocity(v);
            }
            cl = cl->next;
        }
    } else
        patVel = (pos-pp)/elapsed;
}

#pragma mark -
#pragma mark Some other stuff

void PHLObject::updatePosition()
{
	if (!body)
		return;
	b2Vec2 p = body->GetPosition();
    ph_float r = body->GetAngle();
    
    if (correctPos)
    {
        if ((p.x==(float)(cinePos.x)) && (p.y==(float)(cinePos.y)))
            correctPos = false;
        else
        {
            p.x = cinePos.x;
            p.y = cinePos.y;
        }
    }
    if (correctRot) 
    {
        if (r==cineRot)
            correctRot = false;
        else
            r = cineRot;
    }
    if (correctPos || correctRot)
    {
        body->SetTransform(p, r);
        correctPos=correctRot=false;
    }
    
    pos = PHPoint(p.x, p.y);
    rot = r;
    if (shouldFlipUponLoad)
    {
        setFlipped(true);
        shouldFlipUponLoad = false;
    }
}

void PHLObject::updateView(ph_float elapsed, ph_float interpolate)
{
    if (view)
    {
        ph_float rot_adj = 0;
        PHPoint pos_adj(0,0);
        if (body)
        {
            b2Vec2 v = body->GetLinearVelocity();
            pos_adj = PHPoint(v.x * interpolate, v.y * interpolate);
            rot_adj = body->GetAngularVelocity() * interpolate;
        }
        view->setPosition(pos+viewSize+offset+pos_adj);
        view->setRotation(rot+rot_adj);
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
	
	ph_float period = gm->frameInterval();
	
	b2Vec2 v = body->GetLinearVelocity();
	ph_float X,Y;
	X=v.x; Y=v.y;
	if (maxSpeed!=FLT_MAX)
	{
		ph_float length = sqrt(X*X+Y*Y);
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
    contacts.insert(contact);
}

void PHLObject::contactEnd(bool b,b2Contact* contact)
{
    contacts.erase(contact);
}

void PHLObject::contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold)
{
    
}

void PHLObject::contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}

void PHLObject::defferedLoading(PHWorld * _wrld, int insertPos, PHLObject * insObj)
{
    dlipos = insertPos;
    dliobj = insObj;
    dlworld = _wrld;
    wrld = _wrld;
    wrld->viewEventQueue()->schedule(PHInv(this, PHLObject::_defferedLoading, NULL), false);
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

void PHLObject::commitAnimations(ph_float el)
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
            ph_float tm = a->time;            
            ph_float elapsedTime = el;
            if (a->isValid() && tm && !(tm==INFINITY && a->isSkipped()))
            {
                ph_float remaining = tm-a->elapsed;
                if ((jobFinished=((remaining<=elapsedTime)||a->isSkipped())))
                    elapsedTime = remaining;
                if (!a->isSkipped())
                    doneJob = true;
                a->elapsed +=elapsedTime;
                ph_float opos = a->position;
                ph_float pos = a->f((a->elapsed)/tm);
                a->position = pos;
                ph_float dif = pos-opos;
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
                        ph_float desired = frc.length();
                        frc/=desired;
                        b2Vec2 vel = body->GetLinearVelocity();
                        ph_float actual = frc.x*vel.x+frc.y*vel.y;
                        ph_float impulse = (desired - actual)*body->GetMass();
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
                        ph_float vlen = vel.Length();
                        vel*=1/vlen;
                        ph_float impulse = vlen*body->GetMass();
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
    getWorld()->viewEventQueue()->schedule(PHInv(this, PHLObject::_destroy, NULL), false);
}

void PHLObject::_destroy()
{
    getWorld()->removeObject(this);
}

void PHLObject::poof()
{
    getWorld()->viewEventQueue()->schedule(PHInv(this, PHLObject::_poof, NULL), false);
}

void PHLObject::_poof()
{
    if (!poofRect.width || !poofRect.height)
    {
        poofRect = viewSize;
    }
    PHImage * iv = PHPoofView::poofImage(gm);
    ph_float w = iv->width();
    ph_float h = iv->height();
    ph_float dar = w/h;
    ph_float ar = poofRect.width/poofRect.height;
    if (ar>dar)
    {   poofRect.y -= (poofRect.width/dar-poofRect.height)/2;
        poofRect.height = poofRect.width/dar;
    }
    else
    {
        poofRect.x -= (poofRect.height*dar-poofRect.width)/2;
        poofRect.width = poofRect.height*dar;
    }
    if (isFlipped())
    {
        poofRect.x = -poofRect.x-poofRect.width;
    }
    PHPoofView * v = new PHPoofView(poofRect+position(),PHPoofView::poof);
    v->setAnimatorPool(wrld->levelController()->animatorPool());
    getWorld()->getWorldView()->addChild(v);
    v->release();
    
    PHSound * s = gameManager()->soundManager()->soundNamed("poof")->duplicate();
    getWorld()->soundPool()->addSound(s);
    s->playAndRelease(getWorld()->realTimeEventQueue());
    
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
    ph_float rot;
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

static int PHLObject_childWithTag(lua_State * L)
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

PHLuaBoolGetter(PHLObject, isFlipped);
PHLuaBoolSetter(PHLObject, setFlipped);
PHLuaDefineCall(PHLObject, flip);

static int PHLObject_setPatrolPath(lua_State * L)
{
    PHLObject * o = (PHLObject*)PHLuaThisPointer(L);
    lua_pushvalue(L, 2);
    PHBezierPath * bp = PHBezierPath::fromLua(L);
    lua_pop(L, 1);
    o->setPatrolPath(bp);
    if (bp)
        bp->release();
    return 0;
}

PHLuaNumberGetter(PHLObject, patrolSpeed);
PHLuaNumberSetter(PHLObject, setPatrolSpeed);
PHLuaBoolGetter(PHLObject, patrolInCircle);
PHLuaBoolSetter(PHLObject, setPatrolInCircle);
PHLuaNumberGetter(PHLObject, patrolLength)
PHLuaNumberGetter(PHLObject, patrolPosition)
PHLuaNumberSetter(PHLObject, setPatrolPosition)
PHLuaBoolGetter(PHLObject, patrolReversed)
PHLuaBoolSetter(PHLObject, setPatrolReversed)

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
    
    PHLuaAddMethod(PHLObject, childWithTag);
    
    PHLuaAddMethod(PHLObject, invalidateAllAnimations);
    PHLuaAddMethod(PHLObject, skipAllAnimations);
    PHLuaAddMethod(PHLObject, addAnimation);
    
    PHLuaAddMethod(PHLObject, isFlipped);
    PHLuaAddMethod(PHLObject, setFlipped);
    PHLuaAddMethod(PHLObject, flip);
    
    PHLuaAddMethod(PHLObject, setPatrolPath);
    PHLuaAddMethod(PHLObject, patrolSpeed);
    PHLuaAddMethod(PHLObject, setPatrolSpeed);
    PHLuaAddMethod(PHLObject, patrolInCircle);
    PHLuaAddMethod(PHLObject, setPatrolInCircle);
    PHLuaAddMethod(PHLObject, patrolLength)
    PHLuaAddMethod(PHLObject, patrolPosition)
    PHLuaAddMethod(PHLObject, setPatrolPosition)
    PHLuaAddMethod(PHLObject, patrolReversed)
    PHLuaAddMethod(PHLObject, setPatrolReversed)
    
    lua_pop(L, 1);
}

void PHLObject::luaPushSelf(lua_State * l)
{
    if (hasScripting && l==L)
        PHLuaGetWeakRef(L, this);
    else
        lua_pushstring(l, _class.c_str());
}
