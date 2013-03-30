/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLOBJECT_H
#define PHLOBJECT_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHCinematicActor.h>

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
class PHLevelController;
class PHGameManager;
class PHBezierPath;

#define PHL_REGISTEROBJECT(clss) PH_REGISTERCLASS(PHLObject::initMap, #clss, clss)

class PHLObject : public PHObject, public PHCinematicActor
{
protected:
	string _class;
	PHPoint pos;
	float rot;
	float maxSpeed,maxSpeedX,maxSpeedY;
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
    bool drfw;
    
	friend class PHLevelController;
	friend class PHWorld;
	friend class PHJoint;
    
    lua_State * L;
    
    list<b2FixtureDef*>bezierPaths;
    list<b2FixtureDef*>fixturesDefinitions;
    list<b2Fixture*>fixtures;
    void rebuildFixtures();
    void bezierCallback(PHObject * sender, void * ud);
    void rebuildBezier(b2FixtureDef * def);
    void _buildBezier(b2FixtureDef * def);
    
    PHGameManager * gm;
    
    bool flipped;
    bool shouldFlipUponLoad;
    
public:
	PHLObject();
	virtual ~PHLObject();
	const string & getClass() {return _class;};
	PHView * getView() { return view;};
	PHPoint position() { return pos; };
	float rotation() { return rot; };
	float maxVelocity() { return maxSpeed; }
	void setMaxVelocity(float v) { maxSpeed = v; }
	float maxVelocityX() { return maxSpeedX; }
	void setMaxVelocityX(float v) { maxSpeedX = v; }
	float maxVelocityY() { return maxSpeedY; }
	void setMaxVelocityY(float v) { maxSpeedY = v; }
	void temporarilyDisableVelocityLimit() { disableLimit = true; }
    
    virtual void setCinematicPosition(const PHPoint &);
    virtual PHPoint cinematicPosition();
    virtual void setCinematicRotation(float);
    virtual float cinematicRotation();
    virtual void setCinematicScale(const PHSize &) {};
    virtual PHSize cinematicScale() { return PHSize(1,1); }
    virtual void setCinematicBgColor(const PHColor &) {}
    virtual PHColor cinematicBgColor() { return PHInvalidColor; }
    
	void setRotation(float r);
	void setPosition(PHPoint p);
    void setTransform(PHPoint p,float r);
	void limitVelocity();
    bool isDynamic();
    void setDynamic(bool d);
    void rotateAround(float r, PHPoint around);
    
    virtual void flip();
    bool isFlipped() {  return flipped; }
    void setFlipped(bool sf) { if (flipped!=sf) flip(); }
    
    void applyForce(PHPoint force, PHPoint appPoint);
    void applyImpulse(PHPoint impulse, PHPoint appPoint);
    PHPoint velocity();
    float scalarVelocity();
    void setVelocity(PHPoint vel);
    float angularVelocity();
    void setAngularVelocity(float v);
    void applyAngularImpulse(float impulse);
    float mass();
    PHPoint centerOfMass();
    
	
    PHPoint worldPoint(const PHPoint & p);
    PHPoint localPoint(const PHPoint & p);
    PHPoint worldVector(const PHPoint & p);
    PHPoint localVector(const PHPoint & p);
    
	virtual void loadFromLua(lua_State * L, b2World * _world, PHLevelController * lvlc);
	virtual void loadView();
	
    static map<string,PHAllocator> * initMap;
	static PHLObject * objectWithClass(const string & str);
	
    virtual void updatePhysics();
    virtual void updatePosition();
    virtual void updateView(float elapsed, float interpolate);
    
    b2Body * getBody() { return body; }
    
    PHWorld * getWorld() { return wrld; }
    
    void destroy();
    void _destroy();
    void poof();
    
protected:
    PHBezierPath * patrol;
    float patSpeed;
    bool patCircle;
    float patLength;
    float jointLength;
    float patPos;
    PHPoint patLastVel;
    PHPoint patVel;
    bool patRev;
    PHPoint cinePos;
    bool needsCinePos;
    float cineRot;
    bool needsCineRot;
    PHPoint lastVel;
    float lastOmega;
    bool needsLVel;
    bool needsLOmega;
    bool correctPos,correctRot;
    
    void updatePatrol(float elapsed);
    void updateCinematics(float elapsed);
    
    const void * patSignature;
    int patP;
    float lastPos;
public:
    PHBezierPath * patrolPath() { return patrol; }
    void setPatrolPath(PHBezierPath * p);
    float patrolSpeed() { return patSpeed; }
    void setPatrolSpeed(float p) { patSpeed = p; }
    bool patrolInCircle() { return patCircle; }
    void setPatrolInCircle(bool c) { patCircle = c; }
    float patrolLength() { return patLength+(patCircle?jointLength:0); }
    float patrolPosition() { return patPos; }
    void setPatrolPosition(float d) { patPos = d; }
    bool patrolReversed() { return patRev; }
    void setPatrolReversed(bool pr) { patRev = pr; }
    
protected:
    PHPoint offset;
    
protected:
    PHRect poofRect;
    void _poof();
    
    set<b2Contact*> contacts;
    
    virtual bool customizeFixture(lua_State * L, b2FixtureDef & fixtureDef);
    
    virtual bool collidesWith(PHLObject * obj);
    virtual void contactBegin(bool b,b2Contact* contact);
    virtual void contactEnd(bool b,b2Contact* contact);
    virtual void contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold);
    virtual void contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse);
    
    friend class PHContactListener;
    friend class PHContactFilter;
    
public:
    
    void setDontRemoveFromWorld(bool s) { drfw = s; }
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
    void commitAnimations(float elapsedTime);
private:
    list<PHLAnimation*>animations;
public:
    PHGameManager * gameManager() { return gm; }
    void setGameManager(PHGameManager * gameManager) { gm = gameManager; }
};
#endif
