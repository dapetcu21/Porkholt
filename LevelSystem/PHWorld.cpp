/*
 *  PHWorld.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHWorld.h"

#include "PHTimer.h"
#include "PHCaptureView.h"
#include "PHGaugeView.h"
#include "PHImage.h"
#include "PHLevelController.h"
#include <Box2D/Box2D.h>
#include "PHEventQueue.h"

#include "PHLPlayer.h"
#include "PHLNPC.h"
#include "PHLCamera.h"
#include "PHMainEvents.h"

#include "PHDialog.h"
#include "PHScripting.h"

#include "PHTextView.h"

//304 19
#define GAUGE_WIDTH 256
#define GAUGE_HEIGHT 16

class PHContactListener : public b2ContactListener
{
public:
    void BeginContact(b2Contact* contact)
    { 
        PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
        PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
        if (o1)
            o1->contactBegin(false,contact);
        if (o2)
            o2->contactBegin(true,contact);
    }
    
    void EndContact(b2Contact* contact)
    { 
        PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
        PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
        if (o1)
            o1->contactEnd(false,contact);
        if (o2)
            o2->contactEnd(true,contact);
    }
    
    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
    { 
        PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
        PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
        if (o1)
            o1->contactPreSolve(false,contact, oldManifold);
        if (o2)
            o2->contactPreSolve(true,contact, oldManifold);
    }
    
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
    {
        PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
        PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
        if (o1)
            o1->contactPostSolve(false,contact, impulse);
        if (o2)
            o2->contactPostSolve(true,contact, impulse);
    }
};

class PHContactFilter : public b2ContactFilter
{
public:
    bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
    {
        bool collide =  b2ContactFilter::ShouldCollide(fixtureA,fixtureB);
        PHLObject * objA = (PHLObject*)fixtureA->GetBody()->GetUserData();
        PHLObject * objB = (PHLObject*)fixtureB->GetBody()->GetUserData();
        if (objA && objB)
        {
            collide = collide && objA->collidesWith(objB) && objB->collidesWith(objA);
        }
        return collide;
    }
};

PHWorld::PHWorld(const PHRect & size, PHLevelController * cntr) : view(NULL), camera(NULL), player(NULL), _jumpGauge(0.0f), maxJump(100), jumpGrowth(100), controller(cntr), contactFilter(NULL), contactListener(NULL), modelQueue(new PHEventQueue), viewQueue(new PHEventQueue), currentDialog(NULL), dialogInitiator(NULL), dimView(NULL), overlayView(NULL)
{
	PHRect bounds = PHMainEvents::sharedInstance()->screenBounds();
	view = new PHCaptureView(bounds);
	jumpGaugeView = new PHGaugeView(PHMakeRect(bounds.x+2, bounds.height-5-GAUGE_HEIGHT, GAUGE_WIDTH, GAUGE_HEIGHT));
	jumpGaugeView->setImage(PHImage::imageNamed("gauge"));
    PHImageView * frameView = new PHImageView(PHMakeRect(bounds.x,bounds.height-25,bounds.width,25));
    frameView->setImage(PHImage::imageNamed("frame"));
	PHMutex * mutex = new PHMutex();
	view->setMutex(mutex);
    mutex->release();
	view->setQueue(&eventQueue);
	view->setUserInput(true);
	view->setSemaphores(cntr->pSem1, cntr->pSem2);
	view->setPaused(&cntr->paused);
	worldView = new PHView(size);
	worldView->setUserInput(true);
	layerView = new PHView(bounds);
	worldSize = size;
	view->addSubview(layerView);
	view->addSubview(worldView);
    view->addSubview(frameView);
	view->addSubview(jumpGaugeView);
	b2Vec2 grav(0,-10);
	physicsWorld = new b2World(grav,true);
    physicsWorld->SetContactFilter(contactFilter = new PHContactFilter);
    physicsWorld->SetContactListener(contactListener = new PHContactListener);
}

PHWorld::~PHWorld()
{
	removeAllObjects();
    removeAllJoints();
	if (worldView)
	{
		worldView->removeFromSuperview();
		worldView->release();
	}
    if (dimView)
    {
        dimView->removeFromSuperview();
        dimView->release();
    }
    if (overlayView)
    {
        overlayView->removeFromSuperview();
        overlayView->release();
    }
	if (jumpGaugeView)
	{
		jumpGaugeView->removeFromSuperview();
		jumpGaugeView->release();
	}
	if (layerView)
	{
		layerView->removeFromSuperview();
		layerView->release();
	}
	if (view)
		view->release();
	delete physicsWorld;
    delete contactListener;
    delete contactFilter;
    viewQueue->release();
    modelQueue->release();
}

void PHWorld::updatePositions()
{
    double frameInterval = 1.0f/(PHMainEvents::sharedInstance()->framesPerSecond());
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
    {
        PHLObject * obj = *i;
        obj->updatePosition();
        obj->limitVelocity();
        obj->commitAnimations(frameInterval);
    }
    modelQueue->processQueue();
}


void PHWorld::updateScene()
{
    viewQueue->processQueue();
    updateDialogs();
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
    {
        PHLObject * obj = *i;
        obj->updateView();
    }
    camera->updateCamera(player->position());
	if (camera)
	{
		PHRect pos = camera->bounds();
		PHPoint ps = camera->position();
        PHPoint posit = ps;
        double rot = camera->rotation();
		ps.x -= pos.width/2;			
		ps.y -= pos.height/2;
		double scaleX = view->bounds().width / pos.width;
		double scaleY = view->bounds().height / pos.height;
		pos.x = -ps.x * scaleX;
		pos.y = -ps.y * scaleY;
		pos.width = worldSize.width * scaleX;
		pos.height = worldSize.height * scaleY;
		pos.x=round(pos.x);
		pos.y=round(pos.y);
		worldView->setFrame(pos);
		worldView->setBounds(worldSize);
        worldView->setRotationalCenter(posit);
        worldView->setRotation(-rot);
		for (list<layer>::iterator i = layers.begin(); i!=layers.end(); i++)
		{
			layer & ly = *i;
			PHRect pos = camera->bounds();
			PHPoint ps = camera->position();
			ps.x -= pos.width/2;			
			ps.y -= pos.height/2;
			double scaleX = view->bounds().width / pos.width * ly.scale;
			double scaleY = view->bounds().height / pos.height * ly.scale;
			pos.x = -ps.x * scaleX;
			pos.y = -ps.y * scaleY;
			pos.width = worldSize.width * scaleX;
			pos.height = worldSize.height * scaleY;
			ly.container->setFrame(pos);
			ly.container->setBounds(worldSize);
            ly.container->setRotationalCenter(posit);
            ly.container->setRotation(-rot);
		}
	}
	jumpGaugeView->setLevel(_jumpGauge/maxJump);
}

void PHWorld::addObject(PHLObject * obj)
{
	insertObjectAtPosition(obj,0,NULL);
}

void PHWorld::insertObjectAtPosition(PHLObject * obj, int insPos, PHLObject * insObj)
{
    if (!obj) return;
    if (insPos==0)
        objects.push_back(obj);
    else
    if (insPos==1)
        objects.insert(objects.begin(),obj);
    else
    {
        vector<PHLObject*>::iterator i,nx;
        for (i=objects.begin(); i!=objects.end(); i=nx)
        {
            nx=i;
            nx++;
            if (*i==insObj)
            {
                if (insPos==4)
                    i=nx;
                break;
            }
        }
        objects.insert(i,obj);
    }
	obj->retain();
	if (obj->getClass()=="PHLCamera")
		camera = (PHLCamera*)obj;
	if (obj->getClass()=="PHLPlayer")
		player = (PHLPlayer*)obj;
	obj->wrld = this;
	worldView->addSubview(obj->getView());
}

void PHWorld::removeObject(PHLObject * obj)
{
	if (!obj) return;
	if (obj==camera)
		camera = NULL;
	if (obj==player)
		player = NULL;
	for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
		if (*i == obj)
		{
			objects.erase(i);
			obj->wrld = NULL;
			obj->getView()->removeFromSuperview();
			obj->release();
			break;
		}
}

void PHWorld::removeAllObjects()
{
	camera = NULL;
	player = NULL;
	for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
	{
		(*i)->wrld = NULL;
		(*i)->getView()->removeFromSuperview();
		(*i)->release();
	}
	objects.clear();
}

void PHWorld::addJoint(PHJoint * obj)
{
    if (!obj) return;
    joints.push_back(obj);
    obj->retain();
}
void PHWorld::removeJoint(PHJoint * obj)
{
    for (vector<PHJoint*>::iterator i = joints.begin(); i!=joints.end(); i++)
		if (*i == obj)
		{
            joints.erase(i);
            obj->release();
        }
}

void PHWorld::removeAllJoints()
{
    for (vector<PHJoint*>::iterator i = joints.begin(); i!=joints.end(); i++)
    {
        (*i)->release();
    }
    objects.clear();
}

PHWorld::layer * PHWorld::addLayer(double scale)
{
	layer tmp;
	tmp.container = new PHView(worldSize);
	layerView->addSubview(tmp.container);
	tmp.container->release();
	tmp.scale = scale;
	layers.push_back(tmp);
	return &(layers.back());
}

void PHWorld::addToLayer(layer * lyr, PHImageView * img)
{
	lyr->container->addSubview(img);
}

void PHWorld::scheduleTimer(PHTimer * timer)
{
    if (!timer || !timer->isValid())
        return;
    timer->setLastUpdatedAt(PHTime::getTime());
    timers.push_back(timer);
    timer->retain();
}

void PHWorld::updateTimers(double timeElapsed)
{
    double time = PHTime::getTime();
    
    list<PHTimer*>::iterator i,nx;
    for (i=timers.begin(); i!=timers.end(); i=nx)
    {
        nx = i;
        nx++;
        PHTimer * timer = *i;
        double real = time-timer->lastUpdatedAt();
        if (real>timeElapsed)
            real=timeElapsed;
        timer->setLastUpdatedAt(time);
        timer->timePassed(real);
        if (!(timer->isValid()))
        {
            timers.erase(i);
            timer->release();
        }
    }
}

#pragma mark -
#pragma mark dialogs

void PHWorld::advanceDialog()
{
    if (dialogs.empty() && currentDialog)
    {
        currentDialog->npc->setDialog(NULL);
        currentDialog->release();
        currentDialog = NULL;
        return;
    } 
    PHDialog * first = dialogs.front();
    if (currentDialog && currentDialog->npc == first->npc)
        currentDialog->npc->setDialog(first);
    else
    {
        if (currentDialog)
            currentDialog->npc->setDialog(NULL);
        first->npc->setDialog(first);
    }
    if (currentDialog)
        currentDialog->release();
    currentDialog = first;
    dialogs.pop_front();
}

void PHWorld::updateDialogs()
{
    if (!currentDialog)
    {
        if (!dialogs.empty())
        {
            if (!dialogInitiator)
            {
                dialogInitiator = dialogs.front()->npc;
                dialogInitiator->retain();
                dialogInitiator->setInternalShowsQuest(false);
            }
            if (!(dialogInitiator->animatingquest))
                advanceDialog();
        }
        else
        {
            if (dialogInitiator)
            {
                dialogInitiator->setInternalShowsQuest(true);
                dialogInitiator->release();
                dialogInitiator = NULL;
            }
        }
    }
}

void PHWorld::addDialog(PHDialog* d)
{
    dialogs.push_back(d);
    d->retain();
    d->inStack = true;
}

void PHWorld::_fadedToColor(PHObject * obj, void * ud)
{
    if (ud)
        scripting->worldHasFadedAway(ud);
}

void PHWorld::fadeToColor(const PHColor & color, void * ud)
{
    if (color == PHInvalidColor)
    {
        dismissFading();
        return;
    }
    if (!dimView)
    {
        dimView = new PHView(view->bounds());
        view->addSubview(dimView);
    }
    dimColor = color;
    dimView->cancelAnimationsWithTag(5072);
    dimView->setBackgroundColor(PHClearColor);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    anim->time = 0.5;
    anim->view = dimView;
    anim->bgColor = dimColor;
    anim->callback = (PHCallback)&PHWorld::_fadedToColor;
    anim->target = this;
    anim->userdata = ud;
    PHView::addAnimation(anim);
    anim->release();
}

void PHWorld::dismissFading(void * ud)
{
    if (!dimView) return;
    dimView->cancelAnimationsWithTag(5072);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    anim->time = 0.5;
    anim->view = dimView;
    anim->bgColor = PHClearColor;
    anim->callback = (PHCallback)&PHWorld::_fadedToColor;
    anim->target = this;
    anim->userdata = ud;
    PHView::addAnimation(anim);
    anim->release();
}

void PHWorld::_overlayDismissed(PHObject * obj, void * ud)
{
    if (overlayView)
    {
        overlayView->removeFromSuperview();
        overlayView->release();
        overlayView = NULL;
    }
}

void PHWorld::dismissOverlayText()
{
    if (!overlayView) return;
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->customColor = PHClearColor;
    anim->time = 0.5;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    anim->view = overlayView;
    anim->callback = (PHCallback)&PHWorld::_overlayDismissed;
    anim->target = this;
    PHView::addAnimation(anim);
    anim->release();
}

#define OVERLAYFONTSIZE 0.15
#define OVERLAYPOS (5.1f/6)

void PHWorld::overlayText(const string & s, double duration)
{
    if (!overlayView)
    {
        PHRect bounds = view->bounds();
        overlayView = new PHTextView;
        overlayView->setFont(PHFont::fontNamed("ArialBlack"));
        overlayView->setFontSize(OVERLAYFONTSIZE*bounds.height);
        overlayView->setFrame(PHMakeRect(0, (OVERLAYPOS-OVERLAYFONTSIZE/2)*bounds.height, bounds.width, OVERLAYFONTSIZE*bounds.height));
        overlayView->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
        view->addSubview(overlayView);
    }
    overlayView->setFontColor(PHClearColor);
    overlayView->setText(s);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->customColor = PHMakeColor(0.9, 0.9, 0.9, 1);
    anim->time = 0.5;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    anim->view = overlayView;
    PHView::addAnimation(anim);
    anim->release();
    
    PHTimer * timer = new PHTimer;
    timer->setTimeInterval(duration-0.5);
    timer->setRepeats(false);
    timer->setCallback(this, (PHCallback)&PHWorld::dismissOverlayText, NULL);
    scheduleTimer(timer);
    timer->release();
}