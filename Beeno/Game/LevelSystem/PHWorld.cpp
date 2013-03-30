/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHWorld.h"

#include <Porkholt/Core/PHTimer.h>
#include "PHCaptureView.h"
#include "PHGaugeView.h"
#include "PHHeartView.h"
#include <Porkholt/Core/PHImage.h>
#include "PHLevelController.h"
#include <Box2D/Box2D.h>
#include <Porkholt/Core/PHAnimatorPool.h>

#include "PHLPlayer.h"
#include "PHLNPC.h"
#include "PHLMob.h"
#include "PHLCamera.h"
#include <Porkholt/Core/PHGameManager.h>

#include "PHDialog.h"
#include "PHScripting.h"

#include <Porkholt/Core/PHTextView.h>
#include <Porkholt/UI/PHButtonView.h>
#include "PHPoofView.h"

#include <Porkholt/Sound/PHSoundManager.h>
#include "PHShieldView.h"
#include <Porkholt/Sound/PHSoundPool.h>
#include <Porkholt/Sound/PHSound.h>
#include <Porkholt/Core/PHMutex.h>
#include <Porkholt/IO/PHDirectory.h>

//304 19
#define GAUGE_WIDTH (256/480.0f)
#define GAUGE_HEIGHT (16/480.0f)
#define GAUGE_X (2/480.0f)
#define GAUGE_Y (5/480.0f)

#define HEART_HEIGHT (16/480.0f)
#define HEART_X (32/480.0f)
#define HEART_Y (5/480.0f)
#define HEART_GAP (5/480.0f)

#define PAUSE_SIZE (20/480.0f)
#define PAUSE_X (5/480.0f)
#define PAUSE_Y (5/480.0f)


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

PHWorld::PHWorld(PHGameManager * mgr, const PHRect & size, PHLevelController * cntr) : view(NULL), camera(NULL), player(NULL), controller(cntr), contactFilter(NULL), contactListener(NULL), modelQueue(new PHAnimatorPool), viewQueue(new PHAnimatorPool), realQueue(new PHAnimatorPool), sndPool(new PHSoundPool),  currentDialog(NULL), dialogInitiator(NULL), dimView(NULL), overlayView(NULL), gm(mgr)
{
	PHRect bounds = gm->screenBounds();
	view = new PHCaptureView(bounds);
    PHRect gaugeFrame;
	jumpGaugeView = new PHGaugeView(gaugeFrame = PHRect(bounds.x+GAUGE_X*bounds.width, bounds.height-(GAUGE_Y+GAUGE_HEIGHT)*bounds.width, GAUGE_WIDTH*bounds.width, GAUGE_HEIGHT*bounds.width));
	jumpGaugeView->setImage(mgr->imageNamed("gauge"));
    heartView = new PHHeartView(PHRect(gaugeFrame.x+gaugeFrame.width, bounds.height-(HEART_Y+HEART_HEIGHT)*bounds.width, bounds.width-gaugeFrame.x-gaugeFrame.width-HEART_X*bounds.width, HEART_HEIGHT*bounds.width));
    heartView->setHeartGap(HEART_GAP*bounds.width);
    heartView->setHorizontallyFlipped(true);
    heartView->setFlippedOrder(true);
    heartView->setImage(mgr->imageNamed("heart"));
    
    PHButtonView * pauseButton = new PHButtonView(PHRect(bounds.width*(1.0f-PAUSE_SIZE-PAUSE_X),bounds.height-(PAUSE_Y+PAUSE_SIZE)*bounds.width,bounds.width*PAUSE_SIZE,bounds.width*PAUSE_SIZE));
    pauseButton->setImage(mgr->imageNamed("pause"));
    pauseButton->setPressedImage(pauseButton->image());
    pauseButton->setDownCallback(PHInv(controller, PHLevelController::pauseWithMenu, NULL));
    
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
	view->addChild(layerView);
	view->addChild(worldView);
    view->addChild(heartView);
	view->addChild(jumpGaugeView);
    view->addChild(pauseButton);
    pauseButton->release();
	b2Vec2 grav(0,-10);
	physicsWorld = new b2World(grav);
    physicsWorld->SetContactFilter(contactFilter = new PHContactFilter);
    physicsWorld->SetContactListener(contactListener = new PHContactListener);
}

PHWorld::~PHWorld()
{
	removeAllObjects();
    removeAllJoints();
	if (worldView)
	{
		worldView->removeFromParent();
		worldView->release();
	}
    if (dimView)
    {
        dimView->removeFromParent();
        dimView->release();
    }
    if (overlayView)
    {
        overlayView->removeFromParent();
        overlayView->release();
    }
	if (jumpGaugeView)
	{
		jumpGaugeView->removeFromParent();
		jumpGaugeView->release();
	}
    if (heartView)
    {
        heartView->removeFromParent();
        heartView->release();
    }
	if (layerView)
	{
		layerView->removeFromParent();
		layerView->release();
	}
	if (view)
    {
        view->removeFromParent();
		view->release();
    }
	delete physicsWorld;
    delete contactListener;
    delete contactFilter;
    unloadAssets();
    viewQueue->release();
    modelQueue->release();
    realQueue->release();
    sndPool->release();
}

void PHWorld::updatePhysics()
{
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
    {
        PHLObject * obj = *i;
        obj->updatePhysics();
    }
}

void PHWorld::updatePositions()
{
    float frameInterval = gm->frameInterval();
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
    {
        PHLObject * obj = *i;
        obj->updatePosition();
        obj->limitVelocity();
        obj->commitAnimations(frameInterval);
    }
    modelQueue->advanceAnimation(0);
}


void PHWorld::updateScene(float elapsed, float interpolate)
{
    viewQueue->advanceAnimation(elapsed);
    updateDialogs();
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
    {
        PHLObject * obj = *i;
        obj->updateView(elapsed, interpolate);
    }
    //printObjects();
    camera->updateCamera(player->position(), elapsed);
	if (camera)
	{
		PHRect pos = camera->bounds();
		PHPoint ps = camera->position();
        PHPoint posit = ps;
        float rot = camera->rotation();
		ps.x -= pos.width/2;			
		ps.y -= pos.height/2;
		float scaleX = view->bounds().width / pos.width;
		float scaleY = view->bounds().height / pos.height;
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
			float scaleX = view->bounds().width / pos.width * ly.scale;
			float scaleY = view->bounds().height / pos.height * ly.scale;
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

void PHWorld::updateTimers(float frameInterval)
{
    modelQueue->advanceAnimation(frameInterval);
}

void PHWorld::addObject(PHLObject * obj)
{
	insertObjectAtPosition(obj,0,NULL);
}

void PHWorld::insertObjectAtPosition(PHLObject * obj, int insPos, PHLObject * insObj)
{
	obj->wrld = this;
    obj->setGameManager(gm);
    if (!obj) return;
    if (insPos==0)
    {
        objects.push_back(obj);
        worldView->addChild(obj->getView());
    }
    else
    if (insPos==1)
    {
        objects.insert(objects.begin(),obj);
        worldView->addChild(obj->getView());
        obj->getView()->sendToBack();
    }
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
        vector<PHLObject*>::iterator j = i;
        while (j!=objects.end() && ((*j)->getView() == NULL))
            j++;
        PHView * v = (j==objects.end())?NULL:(*j)->getView();
        objects.insert(i,obj);
        worldView->addChildBefore(obj->getView(), v);
    }
	obj->retain();
	if (obj->getClass()=="PHLCamera")
		camera = (PHLCamera*)obj;
	if (obj->getClass()=="PHLPlayer")
		player = (PHLPlayer*)obj;
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
                v->removeFromParent();
            obj->setDontRemoveFromWorld(true);
			obj->release();
			break;
		}
}

void PHWorld::removeAllObjects()
{
    //printObjects();
    camera = NULL;
    player = NULL;
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++) 
	{
        PHLObject * o = *i;
        PHView * v = o->getView();
        if (v)
            v->removeFromParent();
        o->setDontRemoveFromWorld(true);
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

PHWorld::layer * PHWorld::addLayer(float scale)
{
	layer tmp;
	tmp.container = new PHView(worldSize);
	layerView->addChild(tmp.container);
	tmp.container->release();
	tmp.scale = scale;
	layers.push_back(tmp);
	return &(layers.back());
}

void PHWorld::addToLayer(layer * lyr, PHImageView * img)
{
	lyr->container->addChild(img);
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
    viewQueue->scheduleAction(PHInvBind(this,PHWorld::_fadedToColor,ud));
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
        view->addChild(dimView);
    }
    dimView->removeCinematicAnimationsWithTag(5072);
    dimView->setBackgroundColor(PHClearColor);
    dimView->beginCinematicAnimation(0.5,PHCinematicAnimator::FadeOutFunction);
    dimView->animateBgColor(dimColor);
    dimView->animationCallback(PHInv(this,PHWorld::fadedToColor,ud));
    dimView->commitCinematicAnimation();
}

void PHWorld::fadeToColor(const PHColor & color, void * ud)
{
    if (color == PHInvalidColor)
    {
        dismissFading();
        return;
    }
    dimColor = color;
    viewQueue->scheduleAction(PHInvBind(this,PHWorld::_fadeToColor,ud));
}

void PHWorld::_dismissFading(PHObject * sender, void * ud)
{
    if (!dimView) return;
    dimView->removeCinematicAnimationsWithTag(5072);
    dimView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeOutFunction);
    dimView->animateBgColor(PHClearColor);
    dimView->animationCallback(PHInv(this,PHWorld::_fadedToColor,ud));
    dimView->commitCinematicAnimation();
}

void PHWorld::dismissFading(void * ud)
{
    viewQueue->scheduleAction(PHInvBind(this,PHWorld::_dismissFading,ud));
}

void PHWorld::_overlayDismissed(PHObject * obj, void * ud)
{
    if (overlayView)
    {
        overlayView->removeFromParent();
        overlayView->release();
        overlayView = NULL;
    }
}

void PHWorld::dismissOverlayText()
{
    if (!overlayView) return;
    overlayView->mutex()->lock();
    overlayView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeInFunction);
    overlayView->animateCustomColor(PHClearColor);
    overlayView->animationCallback(PHInvN(this,PHWorld::_overlayDismissed));
    overlayView->commitCinematicAnimation();
    overlayView->mutex()->unlock();
}

#define OVERLAYFONTSIZE 0.15
#define OVERLAYPOS (5.1f/6)

void PHWorld::overlayText(const string & s, float duration)
{
    if (!overlayView)
    {
        PHRect bounds = view->bounds();
        overlayView = new PHTextView;
        overlayView->setFont(gm->fontNamed("ArialBlack"));
        overlayView->setFontSize(OVERLAYFONTSIZE*bounds.height);
        overlayView->setFrame(PHRect(0, (OVERLAYPOS-OVERLAYFONTSIZE/2)*bounds.height, bounds.width, OVERLAYFONTSIZE*bounds.height));
        overlayView->setAlignment(PHTextView::alignCenter | PHTextView::justifyCenter);
        overlayView->mutex();
        view->addChild(overlayView);
    }
    overlayView->mutex()->lock();
    overlayView->setFontColor(PHClearColor);
    overlayView->setText(s);
    overlayView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeInFunction);
    overlayView->animateCustomColor(PHColor(0, 0, 0, 0.5));
    overlayView->commitCinematicAnimation();
    
    PHTimer * timer = new PHTimer;
    timer->setDuration(duration-0.5);
    timer->setRepeats(false);
    timer->setCallback(PHInv(this, PHWorld::dismissOverlayText, NULL));
    modelQueue->addAnimator(timer);
    timer->release();
    overlayView->mutex()->unlock();
}

void PHWorld::boom(const PHPoint &location, float magnitude, float damage, float radius)
{
    for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
    {
        PHLObject * o = (*i);
        if ((o==player || dynamic_cast<PHLMob*>(o)!=NULL) && (o->position()-location).length()<=radius)
            ((PHLNPC*)o)->decreaseHP(damage);
        b2Body * b = o->getBody();
        if (b && (b->GetType() == b2_dynamicBody))
        {
            PHPoint p = o->position()-location;
            float d = p.length();
            if (d==0)
            {
                d = 0.1;
                p = PHPoint(0,0.1);
            }
            p.normalize();
            float amm = (magnitude/(d*d));
            if (amm > magnitude*2)
                amm = magnitude*2;
            b2Vec2 im(p.x*amm,p.y*amm);
            b->ApplyLinearImpulse(im,b->GetWorldCenter());
        }
    }
    PHPoofView * bm = new PHPoofView(PHRect(location.x-radius,location.y-radius,radius*2,radius*2),PHPoofView::boom);
    bm->setAnimatorPool(levelController()->animatorPool());
    getWorldView()->addChild(bm);
    bm->release();
    PHSound * s = gm->soundManager()->soundNamed("boom")->copy();
    sndPool->addSound(s);
    s->playAndRelease();
}

PHDirectory * PHWorld::resourceDirectory()
{
    return levelController()->bundleDirectory();
}

string PHWorld::resourcePath()
{
    string rp = resourceDirectory()->path();
    string grp = gm->resourceDirectory()->path();
    size_t n = grp.length();
    if (grp == rp.substr(0, n))
        return rp.substr(n);
    else 
        return rp;
}

#define retainImage(fname) { PHImage * i = gm->imageNamed(fname); if (i) { if(preloadedImages.insert(i).second) i->retain(); } }

#define retainImg(image) { PHImage * i = (image); if (i) { if(preloadedImages.insert(i).second) i->retain(); } }

#define retainSound(fname) { PHSound * s = gm->soundManager()->soundNamed(fname); if (s) { if(preloadedSounds.insert(s).second) s->retain(); } }

#define retainSnd(snd) { PHSound * s = (snd); if (s) { if(preloadedSounds.insert(s).second) s->retain(); } }

void PHWorld::preloadAssets()
{
    retainImage("dialogbubble");
    retainImage("quest");
    retainImg(PHPoofView::poofImage(gm));
    retainImg(PHPoofView::boomImage(gm));
    retainImg(PHShieldView::shieldImage(gm));
    retainSound("boom");
    retainSound("poof");
}

void PHWorld::unloadAssets()
{
    for (set<PHImage*>::iterator i = preloadedImages.begin(); i!=preloadedImages.end(); i++)
        (*i)->release();
    preloadedImages.clear();
    for (set<PHSound*>::iterator i = preloadedSounds.begin(); i!=preloadedSounds.end(); i++)
        (*i)->release();
    preloadedSounds.clear();
}

/*void PHWorld::printObjects()
{
    PHLog("objects: p:%x c:%x n:%d",player,camera,objects.size());
    for (int i=0; i<objects.size(); i++)
        PHLog("%x",objects[i]);
    PHLog("------");
}*/
