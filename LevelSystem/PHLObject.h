/*
 *  PHLObject.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHLOBJECT_H
#define PHLOBJECT_H

#define INSIDE_PHLOBJECT_H

#include "PHMain.h"

class PHImageView;
class PHJoint;
class b2World;
class b2Body;
class b2Contact;
class b2Manifold;
class b2ContactImpulse;

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
    
	friend class PHLevelController;
	friend class PHWorld;
	friend class PHJoint;
    
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
	void limitVelocity();
	
	virtual void loadFromLua(void * L, const string & root, b2World * world);
	virtual void loadView();
	
	static PHLObject * objectWithClass(const string & str);
	
	void updatePosition();
    
    b2Body * getBody() { return body; }
    
    virtual bool collidesWith(PHLObject * obj);
    virtual void contactBegin(bool b,b2Contact* contact);
    virtual void contactEnd(bool b,b2Contact* contact);
    virtual void contactPreSolve(bool b,b2Contact* contact, const b2Manifold* oldManifold);
    virtual void contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse);
};
#undef INSIDE_PHLOBJECT_H
#endif