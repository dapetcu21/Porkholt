/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEvent.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PHMutex.h>

PHEvent * PHEventHandler::eventForUD(void * ud)
{
    mutex->lock();
    map<void*,PHEvent*>::iterator i = events.find(ud);
    PHEvent * evt;
    if (i==events.end())
    {
        events[ud] = evt = new PHEvent();
    }
    else
    {
        evt = new PHEvent(i->second);
        i->second->release();
        i->second = evt;
    }
    evt->retain();
    evt->setUserData(ud);
    mutex->unlock();
    return evt;
}

void PHEventHandler::touchDown(PHPoint pnt,void * ud)
{
    PHEvent * evt = eventForUD(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchDown);
    addEvent(evt);
}

void PHEventHandler::touchMoved(PHPoint pnt, void * ud)
{
    PHEvent * evt = eventForUD(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchMoved);
    addEvent(evt);
}

void PHEventHandler::touchUp(PHPoint pnt, void * ud)
{
    PHEvent * evt = eventForUD(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchUp);
    addEvent(evt);
    mutex->lock();
    evt->release();
    events.erase(ud);
    mutex->unlock();
}

void PHEventHandler::touchCancelled(PHPoint pnt, void * ud)
{
    PHEvent * evt = eventForUD(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchCancelled);
    addEvent(evt);
    evt->release();
    events.erase(ud);
}

void PHEventHandler::scrollWheel(PHPoint pnt, PHPoint delta, void *ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setDelta(delta);
    evt->setType(PHEvent::scrollWheel);
    addEvent(evt);
}

void PHEventHandler::pinchZoom(PHPoint pnt, ph_float zoom, void *ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setZoom(zoom);
    evt->setType(PHEvent::pinchZoom);
    addEvent(evt);
}

void PHEventHandler::pinchRotate(PHPoint pnt, ph_float rotation, void *ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setRotation(rotation);
    evt->setType(PHEvent::pinchRotate);
    addEvent(evt);
}

void PHEventHandler::multitouchBegin(void *ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setType(PHEvent::multitouchBegin);
    addEvent(evt);
}

void PHEventHandler::multitouchEnd(void *ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setType(PHEvent::multitouchEnd);
    addEvent(evt);
}

void PHEventHandler::registerForMultitouchEvents(PHDrawable * v)
{
    mtlisteners.insert(v);
}

void PHEventHandler::unregisterForMultitouchEvents(PHDrawable * v)
{
    mtlisteners.erase(v);
}

void PHEventHandler::addEvent(PHEvent * evt)
{
    mutex->lock();
    evtqueue.push_back(evt);
    mutex->unlock();
}

void PHEventHandler::removeDrawable(PHDrawable * d)
{
    unregisterForMultitouchEvents(d);
    mutex->lock();
	for (map<void*,PHEvent*>::iterator i = events.begin(); i!= events.end(); i++)
	{
        PHEvent * e = i->second;
		if (e->owner() == d)
			e->setOwner(NULL);
	}
    for (list<PHEvent*>::iterator i = evtqueue.begin(); i!= evtqueue.end(); i++)
        if ((*i)->owner() == d)
            (*i)->setOwner(NULL);
    mutex->unlock();
}

void PHEventHandler::processQueue()
{
    while (1)
    {
        PHEvent * evt = NULL;
        mutex->lock();
        if (!evtqueue.empty())
        {
            evt = evtqueue.front();
            evtqueue.pop_front();
        }
        mutex->unlock();
        if (!evt) break;

        #ifdef PH_DEBUG
        //PHLog("event: %f %f  type: %d ud: %p   owner: %p", evt->location().x, evt->location().y, evt->type(), evt->userData(), evt->owner());
        #endif

        PHDrawable * firstHandler = gm->mainDrawable();
        switch (evt->type())
        {
            case PHEvent::touchMoved:
            case PHEvent::touchUp:
            case PHEvent::touchCancelled:
                firstHandler = evt->owner();
            case PHEvent::touchDown:
            case PHEvent::pinchZoom:
            case PHEvent::pinchRotate:
            case PHEvent::scrollWheel:
                if (firstHandler)
                    firstHandler->handleEvent(evt);
                break;
            case PHEvent::multitouchBegin:
            case PHEvent::multitouchEnd:
                for (set<PHDrawable*>::iterator i = mtlisteners.begin(); i != mtlisteners.end(); i++)
                    (*i)->handleEvent(evt);
        }

        evt->release();
    }
}

PHEventHandler::PHEventHandler(PHGameManager * gameManager) : gm(gameManager), mutex(new PHMutex) 
{
}

PHEventHandler::~PHEventHandler()
{
    mutex->lock();
    for (list<PHEvent*>::iterator i = evtqueue.begin(); i != evtqueue.end(); i++)
        (*i)->release();
    for (map<void*, PHEvent*>::iterator i = events.begin(); i != events.end(); i++)
        i->second->release();
    mutex->unlock();
    mutex->release();
}

