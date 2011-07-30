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
#include "PHHeartView.h"
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
#include "PHEventQueue.h"

//304 19
#define GAUGE_WIDTH (256/480.0f)
#define GAUGE_HEIGHT (16/480.0f)
#define GAUGE_X (2/480.0f)
#define GAUGE_Y (5/480.0f)

#define HEART_HEIGHT (16/480.0f)
#define HEART_X (5/480.0f)
#define HEART_Y (5/480.0f)
#define HEART_GAP (5/480.0f)

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

PHWorld::PHWorld(const PHRect & size, PHLevelController * cntr) : view(NULL), camera(NULL), player(NULL), controller(cntr), contactFilter(NULL), contactListener(NULL), modelQueue(new PHEventQueue), viewQueue(new PHEventQueue), currentDialog(NULL), dialogInitiator(NULL), dimView(NULL), overlayView(NULL)
{
	PHRect bounds = PHMainEvents::sharedInstance()->screenBounds();
	view = new PHCaptureView(bounds);
    PHRect gaugeFrame;
	jumpGaugeView = new PHGaugeView(gaugeFrame = PHRect(bounds.x+GAUGE_X*bounds.width, bounds.height-(GAUGE_Y+GAUGE_HEIGHT)*bounds.width, GAUGE_WIDTH*bounds.width, GAUGE_HEIGHT*bounds.width));
	jumpGaugeView->setImage(PHImage::imageNamed("gauge"));
    heartView = new PHHeartView(PHRect(gaugeFrame.x+gaugeFrame.width, bounds.height-(HEART_Y+HEART_HEIGHT)*bounds.width, bounds.width-gaugeFrame.x-gaugeFrame.width-HEART_X*bounds.width, HEART_HEIGHT*bounds.width));
    heartView->setHeartGap(HEART_GAP*bounds.width);
    heartView->setHorizontallyFlipped(true);
    heartView->setFlippedOrder(true);
    heartView->setImage(PHImage::imageNamed("heart"));
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
    view->addSubview(heartView);
	view->addSubview(jumpGaugeView);
	b2Vec2 grav(0,-10);
	physicsWorld = new b2World(grav,true);
    physicsWorld->SetContactFilter(contactFilter = new PHContactFilter);
    physicsWorld->SetContactListener(contactListener = new PHContactListener);
}

PHWorld::~PHWorld()
{
    invalidateAllTimers();
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
    if (heartView)
    {
        heartView->removeFromSuperview();
        heartView->release();
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
    if (!player || player->isBarHidden() || !player->usesForce())
        jumpGaugeView->setTintColor(PHClearColor);
    else
    {
        jumpGaugeView->setLevel(player->forceGauge()/player->maximumForce());
        jumpGaugeView->setTintColor(PHInvalidColor);
    }
    int hn = round(player->maximumHP());
    heartView->setHeartNumber(hn);
    heartView->setActiveHearts(round(player->healthPoints()/player->maximumHP()*hn));
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
            PHView * v = obj->getView();
            if (v)
                v->removeFromSuperview();
			obj->release();
			break;
		}
}

void PHWorld::removeAllObjects()
{
	camera = NULL;
	player = NULL;
    int n = objects.size();
	for (int i = 0; i<n; i++)
	{
        PHLObject * o = objects[i];
        if (o==NULL) continue;
        PHView * v = o->getView();
        if (v)
            v->removeFromSuperview();
		o->release();
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

void PHWorld::scheduleTimer(PHTimer * timer, void * ud)
{
    if (!timer || !timer->isValid())
        return;
    timer->setLastUpdatedAt(PHTime::getTime());
    timers.insert(make_pair<void *,PHTimer*>(ud,timer));
    timer->retain();
}

void PHWorld::invalidateAllTimers()
{
    for(multimap<void *,PHTimer*>::iterator i=timers.begin(); i!=timers.end(); i++)
        i->second->release();
    timers.clear();
}

void PHWorld::invalidateTimersWithUserdata(void * ud)
{
    pair<multimap<void *,PHTimer*>::iterator,multimap<void *,PHTimer*>::iterator> range = timers.equal_range(ud);
    for(multimap<void *,PHTimer*>::iterator i = range.first; i!=range.second; i++)
        i->second->release();
    timers.erase(range.first,range.second);
}

void PHWorld::updateTimers(double timeElapsed)
{
    double time = PHTime::getTime();
    
    multimap<void *,PHTimer*>::iterator i,nx;
    for (i=timers.begin(); i!=timers.end(); i=nx)
    {
        nx = i;
        nx++;
        PHTimer * timer = i->second;
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

void PHWorld::fadedToColor(PHObject * obj, void * ud)
{
    viewQueue->schedule(this,(PHCallback)&PHWorld::_fadedToColor,ud,false);
}


void PHWorld::_fadedToColor(PHObject * obj, void * ud)
{
    if (ud)
        scripting->worldHasFadedAway(ud);
}

void PHWorld::_fadeToColor(PHObject * sender, void * ud)
{
    if (!dimView)
    {
        dimView = new PHView(view->bounds());
        view->addSubview(dimView);
    }
    dimView->cancelAnimationsWithTag(5072);
    dimView->setBackgroundColor(PHClearColor);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    anim->time = 0.5;
    anim->view = dimView;
    anim->bgColor = dimColor;
    anim->callback = (PHCallback)&PHWorld::fadedToColor;
    anim->target = this;
    anim->userdata = ud;
    PHView::addAnimation(anim);
    anim->release();
}

void PHWorld::fadeToColor(const PHColor & color, void * ud)
{
    if (color == PHInvalidColor)
    {
        dismissFading();
        return;
    }
    dimColor = color;
    viewQueue->schedule(this,(PHCallback)&PHWorld::_fadeToColor,ud,false);
}

void PHWorld::_dismissFading(PHObject * sender, void * ud)
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

void PHWorld::dismissFading(void * ud)
{
    viewQueue->schedule(this,(PHCallback)&PHWorld::_dismissFading,ud,false);
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
    overlayView->mutex()->lock();
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->customColor = PHClearColor;
    anim->time = 0.5;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    anim->view = overlayView;
    anim->callback = (PHCallback)&PHWorld::_overlayDismissed;
    anim->target = this;
    PHView::addAnimation(anim);
    anim->release();
    overlayView->mutex()->unlock();
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
        overlayView->setFrame(PHRect(0, (OVERLAYPOS-OVERLAYFONTSIZE/2)*bounds.height, bounds.width, OVERLAYFONTSIZE*bounds.height));
        overlayView->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
        overlayView->mutex();
        view->addSubview(overlayView);
    }
    overlayView->mutex()->lock();
    overlayView->setFontColor(PHClearColor);
    overlayView->setText(s);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->customColor = PHColor(0, 0, 0, 0.5);
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
    overlayView->mutex()->unlock();
}

const string & PHWorld::resourcePath()
{
    return levelController()->bundlePath();
}