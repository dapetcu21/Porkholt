/*
 *  PHLPlayer.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHLPlayer.h"
#include "PHAuxLayerView.h"
#include "PHPlayerView.h"
#include "PHWorld.h"
#include "PHTrailImageView.h"
#include "PHMainEvents.h"
#include <Box2D/Box2D.h>
#include "PHMotion.h"

PHView * playerView = NULL;

PHLPlayer::PHLPlayer() : bodyView(NULL), worldView(NULL), faceView(NULL), touchesSomething(false), normal(PHOriginPoint), forceGap(0), trailSize(0), horizontallyFlipped(false), mutex(NULL)
{
	_class = "PHLPlayer";
}

PHLPlayer::~PHLPlayer()
{
    if (mutex)
        mutex->release();
}

void PHLPlayer::loadFromLua(void * L, const string & root,b2World * world)
{
	PHLObject::loadFromLua(L,root,world);
    body->SetBullet(true);
}

void PHLPlayer::loadView()
{
	view = new PHPlayerView(PHMakeRect(viewSize.x+pos.x, viewSize.y+pos.y, viewSize.width, viewSize.height));
    view->setRotationalCenter(PHMakePoint(-viewSize.x, -viewSize.y));
	loadImages();
    ((PHPlayerView*)view)->setDesignatedView(bodyView = (PHTrailImageView*)(view->viewWithTag(20)));
    faceView = (PHImageView*)(view->viewWithTag(21));
    bodyView->setSnapshotInterval(2/(60/PHMainEvents::sharedInstance()->framesPerSecond()));
	view->setRotation(rot);
	playerView = view;
}

#define MAX_TILT 30.0f
#define TILT_FORCE_FACTOR 0.05f
#define TOUCH_FORCE_FACTOR 100.0f
void PHLPlayer::updateControls(list<PHPoint> * queue)
{
	if (!body) return;
	if (!worldView)
    {
        bodyView->setStopView(worldView = (bodyView->superview()->superview()));
        bodyView->bindToAuxLayer(PHAuxLayerView::auxLayerViewWithName(20), worldView);
    }
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
    bool forceUsed = !(queue->empty());
    b2Vec2 totalJump(0,0);
    if (mutex)
        mutex->lock();
	while (!queue->empty()) {
		force.x = queue->front().x*TOUCH_FORCE_FACTOR;
		force.y = queue->front().y*TOUCH_FORCE_FACTOR;
        queue->pop_front();
        if (mutex)
            mutex->unlock();
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
			}
			jumpGauge -= length;
		}
        force*=1/60.0f; //leave it 60... the fps doesn't matter
		body->ApplyLinearImpulse(force, center);
        if (mutex)
            mutex->lock();
	}
    if (mutex)
        mutex->unlock();
    if (forceUsed)
    {
        totalJump.Normalize();
        if (totalJump.y>0.1 && normal.y>0)
        {
            b2Vec2 imp(0,min<double>(normal.y*2,10.0f/fps));
            body->ApplyLinearImpulse(imp,center);
        }
    }
    normal.x = normal.y = 0;
    if (touchesSomething>0)
    {
        jumpGauge+=touchesSomething * wrld->jumpGaugeGrowth()/(double)fps;
        double max = wrld->maxJumpGauge();
        if (jumpGauge > max)
            jumpGauge = max;
        touchesSomething -= 1.0f/(double)fps;
    }
	wrld->setJumpGauge(jumpGauge);
    if (forceUsed)
        forceGap = 4;
    trailSize = forceGap?10:0;
    b2Vec2 speed = body->GetLinearVelocity();
    if (abs(speed.x)>=0.1)
        horizontallyFlipped = (speed.x<0);
    if (forceGap>0)
        forceGap--;
}

void PHLPlayer::updateView()
{
    PHLObject::updateView();
    faceView->setHorizontallyFlipped(horizontallyFlipped);
    bodyView->setTrailSize(trailSize);
}

void PHLPlayer::contactPostSolve(bool b,b2Contact* contact, const b2ContactImpulse* impulse)
{
    touchesSomething = 1.0f;
    b2Manifold * man = contact->GetManifold();
    b2WorldManifold wMan;
    contact->GetWorldManifold(&wMan);
    int n = man->pointCount;
    b2Vec2 norm = wMan.normal;
    for (int i=0; i<n; i++)
    {
        float val = impulse->normalImpulses[i];
        normal.x+=val*norm.x;
        normal.y+=val*norm.y;
    }
}