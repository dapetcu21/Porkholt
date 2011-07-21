/*
 *  PHLObject.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHLOBJECT_H
#define PHLOBJECT_H

#include "PHMain.h"

class PHImageView;
class PHJoint;
class b2World;
class b2Body;
class b2Contact;
class b2Manifold;
class b2ContactImpulse;
class PHWorld;
class PHView;
class PHLAnimation;
struct b2FixtureDef;
class b2Fixture;

class PHLObject : public PHObject
{
protected:
	string _class;
	PHPoint pos;
	double rot;
	double maxSpeed,maxSpeedX,maxSpeedY;
	bool disableLimit;
	PHView * view;
	
	struct Image
	{
		PHImageView * img;
		PHRect bounds;
	};
	list<Image> images;
	PHRect viewSize;
	void loadImages();
	void loadBody(void *l);
	
	PHWorld * wrld;
	b2World * world;
	b2Body * body;
	
    list<PHJoint*> joints;
    void addJoint(PHJoint * joint);
    void removeJoint(PHJoint * joint);
    
    bool hasScripting;
    string scriptingName;
    
	friend class PHLevelController;
	friend class PHWorld;
	friend class PHJoint;
    
    lua_State * L;
    
    vector<b2FixtureDef*>fixturesDefinitions;
    vector<b2Fixture*>fixtures;
    void rebuildFixtures();
    
public:
	PHLObject();
	virtual ~PHLObject();
	const string & getClass() {return _class;};
	PHView * getView() { return view;};
	PHPoint position() { return pos; };
	double rotation() { return rot; };
	double maxVelocity() { return maxSpeed; }
	void setMaxVelocity(double v) { maxSpeed = v; }
	double maxVelocityX() { return maxSpeedX; }
	void setMaxVelocityX(double v) { maxSpeedX = v; }
	double maxVelocityY() { return maxSpeedY; }
	void setMaxVelocityY(double v) { maxSpeedY = v; }
	void temporarilyDisableVelocityLimit() { disableLimit = true; }
	void setRotation(double r);
	void setPosition(PHPoint p);
    void setTransform(PHPoint p,double r);
	void limitVelocity();
    bool isDynamic();
    void setDynamic(bool d);
    void rotateAround(double r, PHPoint around);
    
    void applyForce(PHPoint force, PHPoint appPoint);
    void applyImpulse(PHPoint impulse, PHPoint appPoint);
    PHPoint velocity();
    double scalarVelocity();
    void setVelocity(PHPoint vel);
    double angularVelocity();
    void setAngularVelocity(double v);
    void applyAngularImpulse(double impulse);
    double mass();
    PHPoint centerOfMass();
    
	
    PHPoint worldPoint(const PHPoint & p);
    PHPoint localPoint(const PHPoint & p);
    PHPoint worldVector(const PHPoint & p);
    PHPoint localVector(const PHPoint & p);
    
	virtual void loadFromLua(lua_State * L, const string & root, b2World * world);
	virtual void loadView();
	
	static PHLObject * objectWithClass(const string & str);
	
    virtual void updatePosition();
    virtual void updateView();
    
    b2Body * getBody() { return body; }
    
    PHWorld * getWorld() { return wrld; }
    
private:
    
    virtual bool customizeFixture(lua_State * L, b2FixtureDef & fixtureDef);
    
    virtual bool collidesWith(PHLObject * obj);
    virtual void contactBegin(bool b,b2Contact* contact);
    virtual void contactEnd(bool b,b2Contact* contact);
    virtual void contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold);
    virtual void contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse);
    
    friend class PHContactListener;
    friend class PHContactFilter;
    
public:
    
    void scriptingCreate(lua_State * L);
    void scriptingDestroy();
    void scriptingInit(lua_State * L);
    static void registerLuaInterface(lua_State * L);
    
    void defferedLoading(PHWorld * wrld, int insertPos, PHLObject * insObj);
    void _defferedLoading(PHObject * sender, void * ud);
    
    void luaPushSelf(lua_State * L);
private:
    int dlipos;
    PHLObject * dliobj;
    PHWorld * dlworld;
    
public:
    void addAnimation(PHLAnimation * anim);
    void skipAllAnimations();
    void invalidateAllAnimations();
    void commitAnimations(double elapsedTime);
private:
    list<PHLAnimation*>animations;

};
#endif