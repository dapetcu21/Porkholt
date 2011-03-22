/*
 *  PHWorld.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#ifndef PHWORLD_H
#define PHWORLD_H

#include <Box2D/Box2D.h>

class PHLCamera;
class PHLObject;
class PHLPlayer;
class PHCaptureView;
class PHGaugeView;

class PHWorld : public PHObject 
{
private:
	PHCaptureView * view;
	PHView * worldView;

	PHWorld * wrld;
	PHLCamera * camera;
	PHLPlayer * player;
	
	PHRect worldSize;
	
	b2World * physicsWorld;
	
	list<PHLObject*> objects;
	list<PHPoint> eventQueue;
	
	PHMutex * controlsMutex;
	
	friend class PHLevelController;
	
	double _jumpGauge,maxJump,jumpGrowth;
	PHGaugeView * jumpGaugeView;
public:
	PHWorld(const PHRect & size,PHMutex * mutex);
	PHView * getView() { return (PHView *)view; }
	virtual ~PHWorld();
	
	void addObject(PHLObject * obj);
	void removeObject(PHLObject * obj);
	void removeAllObjects();
	
	void updateScene(double time);
	
	double jumpGauge() { return _jumpGauge; }
	void setJumpGauge(double j) { _jumpGauge = j; }
	double maxJumpGauge() { return maxJump; }
	void setMaxJumpGauge(double j) { maxJump = j; }
	double jumpGaugeGrowth() { return jumpGrowth; }
	void setJumpGaugeGrowth(double g) { jumpGrowth = g; }
};

#endif