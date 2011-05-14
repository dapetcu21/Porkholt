/*
 *  PHLPlayer.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include <Box2D/Box2D.h>

PHView * playerView = NULL;

PHLPlayer::PHLPlayer()
{
	_class = "PHLPlayer";
}

PHLPlayer::~PHLPlayer()
{
}

void PHLPlayer::loadFromLua(void * L, const string & root,b2World * world)
{
	PHLObject::loadFromLua(L,root,world);
}

void PHLPlayer::loadView()
{
	PHLObject::loadView();
	playerView = view;
}

#define MAX_TILT 30.0f
#define TILT_FORCE_FACTOR 0.05f
#define TOUCH_FORCE_FACTOR 100.0f
void PHLPlayer::updateControls(list<PHPoint> * queue)
{
	if (!body) return;
	
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	
	b2Vec2 force;
	PHTilt t = PHMotion::sharedInstance()->getTilt();
	int f = t.roll;
	if (f>MAX_TILT)
		f = MAX_TILT;
	if (f<-MAX_TILT)
		f = -MAX_TILT;
	force.y = 0;
	force.x = f*TILT_FORCE_FACTOR;
	b2Vec2 center = body->GetWorldCenter();
	body->ApplyForce(force, center);
	double jumpGauge = wrld->jumpGauge();
	while (!queue->empty()) {
		force.x = queue->front().x*TOUCH_FORCE_FACTOR;
		force.y = queue->front().y*TOUCH_FORCE_FACTOR;
        queue->pop_front();
		double length = sqrt(force.x*force.x+force.y*force.y);
		if (force.y<0)
			length = fabs(force.x);
		if (length)
		{
			if (length>jumpGauge)
			{
				force.x*=jumpGauge/length;
				if (force.y>0)
					force.y*=jumpGauge/length;
				length = jumpGauge;
			} else
				temporarilyDisableVelocityLimit();
			jumpGauge -= length;
		}
		force.x/=60;
		force.y/=60;
		body->ApplyLinearImpulse(force, center);
	}
	jumpGauge+=wrld->jumpGaugeGrowth()/(double)fps;
	double max = wrld->maxJumpGauge();
	if (jumpGauge > max)
		jumpGauge = max;
	wrld->setJumpGauge(jumpGauge);
}