/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEvent.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHTime.h>
#include <Porkholt/Core/PHMutex.h>

struct PHEventProps
{
    PHPoint loc;
    ph_float time;
    PHDrawable * owner;
};

void PHEventHandler::touchDown(PHPoint pnt,void * ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchDown);
    addEvent(evt);
}

void PHEventHandler::touchMoved(PHPoint pnt, void * ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchMoved);
    addEvent(evt);
}

void PHEventHandler::touchUp(PHPoint pnt, void * ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchUp);
    addEvent(evt);
}

void PHEventHandler::touchCancelled(PHPoint pnt, void * ud)
{
    PHEvent * evt = new PHEvent();
    evt->setUserData(ud);
    evt->setTime(PHTime::getTime());
    evt->setLocation(pnt);
    evt->setType(PHEvent::touchCancelled);
    addEvent(evt);
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
	for (map<void*,PHEventProps*>::iterator i = props.begin(); i!= props.end(); i++)
	{
        PHEventProps * p = i->second;
		if (p->owner == d)
            p->owner = NULL;
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

        PHDrawable * firstHandler = gm->mainDrawable();

        bool destroy = false;
        switch (evt->type())
        {
            case PHEvent::touchCancelled:
            case PHEvent::touchUp:
                destroy = true;
            case PHEvent::touchMoved:
            {
                map<void*, PHEventProps*>::iterator i = props.find(evt->userData());
                if (i!=props.end())
                {
                    PHEventProps * prop = i->second;
                    evt->setLastLocation(prop->loc);
                    evt->setLastTime(prop->time);
                    evt->setOwner(prop->owner);
                    prop->loc = evt->location();
                    prop->time = evt->time();
                    break;
                }
            }
            case PHEvent::touchDown:
            {
                PHEventProps * prop = new PHEventProps;
                prop->loc = evt->location();
                prop->time = evt->time();
                prop->owner = evt->owner();
                props.insert(make_pair(evt->userData(), prop));
                break;
            }
        }
        if (destroy)
        {
            map<void*, PHEventProps*>::iterator i = props.find(evt->userData());
            if (i!=props.end())
            {
                delete i->second;
                props.erase(i);
            }
        }
        
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

        switch (evt->type())
        {
            case PHEvent::touchMoved:
            case PHEvent::touchDown:
            {
                map<void*, PHEventProps*>::iterator i = props.find(evt->userData());
                if (i!=props.end())
                    i->second->owner = evt->owner();
            }
        }

        #ifdef PH_DEBUG
        //PHLog("event: %f %f  type: %d ud: %p   owner: %p", evt->location().x, evt->location().y, evt->type(), evt->userData(), evt->owner());
        #endif

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
    for (map<void*, PHEventProps*>::iterator i = props.begin(); i != props.end(); i++)
        delete i->second;
    mutex->unlock();
    mutex->release();
}

