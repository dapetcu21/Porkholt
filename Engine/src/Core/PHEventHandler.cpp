/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHEventHandler.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHEvent.h>
#include <Porkholt/Core/PHView.h>

PHEvent * PHEventHandler::touchForUserData(void * ud, map<void*,PHEvent*>::iterator & i)
{
    i = events.find(ud);
    if (i==events.end())
        return NULL;
    else
        return i->second;
}

void PHEventHandler::touchDown(PHPoint pnt,void * ud)
{
    map<void*,PHEvent*>::iterator i;
    PHEvent * event = touchForUserData(ud, i);
    if (!event)
    {
        event = new PHEvent();
        events.insert(pair<void*,PHEvent*>(ud,event));
    }
    event->setHandled(false);
	event->updateLocation(pnt, PHTime::getTime(), PHEvent::touchDown);
	event->ud = ud;
    PHView * v = gm->mainView()->pointerDeepFirst(PHIdentityMatrix, event);
    if (!event->_ownerView)
        event->_ownerView = v;
}

void PHEventHandler::touchUp(PHPoint pnt, void * ud)
{
	map<void*,PHEvent*>::iterator i;
	PHEvent * event = touchForUserData(ud,i);
	if (!event) return;
    event->setHandled(false);
	event->updateLocation(pnt, PHTime::getTime(), PHEvent::touchUp);
	if (event->_ownerView)
		event->_ownerView->touchEvent(event);
	events.erase(i);
	event->release();
}

void PHEventHandler::touchMoved(PHPoint pnt, void * ud)
{
	map<void*,PHEvent*>::iterator i;
	PHEvent * event = touchForUserData(ud,i);
	if (!event) return;
    event->setHandled(false);
	event->updateLocation(pnt, PHTime::getTime(), PHEvent::touchMoved);
	if (event->_ownerView)
		event->_ownerView->touchEvent(event);
}

void PHEventHandler::touchCancelled(PHPoint pnt, void *ud)
{
	map<void*,PHEvent*>::iterator i;
	PHEvent * event = touchForUserData(ud,i);
	if (!event) return;
    event->setHandled(false);
	event->updateLocation(pnt, PHTime::getTime(), PHEvent::touchCancelled);
	if (event->_ownerView)
		event->_ownerView->touchEvent(event);
	events.erase(i);
	event->release();
}

void PHEventHandler::scrollWheel(PHPoint pnt, PHPoint delta, void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->_delta = delta;
    event->_location = pnt;
    event->state = PHEvent::scrollWheel;
    PHView * v = gm->mainView()->pointerDeepFirst(PHIdentityMatrix, event);
    if (!event->_ownerView)
        event->_ownerView = v;
    event->release();
}

void PHEventHandler::pinchZoom(PHPoint pnt, ph_float zoom, void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->_zoom = zoom;
    event->_location = pnt;
    event->state = PHEvent::pinchZoom;
    PHView * v = gm->mainView()->pointerDeepFirst(PHIdentityMatrix, event);
    if (!event->_ownerView)
        event->_ownerView = v;
    event->release();
}

void PHEventHandler::pinchRotate(PHPoint pnt, ph_float rotation, void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->_rotation = rotation;
    event->_location = pnt;
    event->state = PHEvent::pinchRotate;
    PHView * v = gm->mainView()->pointerDeepFirst(PHIdentityMatrix, event);
    if (!event->_ownerView)
        event->_ownerView = v;
    event->release();
}

void PHEventHandler::multitouchBegin(void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->state = PHEvent::multitouchBegin;
    for (set<PHView*>::iterator i = mtviews.begin(); i!=mtviews.end(); i++)
        (*i)->touchEvent(event);
    event->release();   
}

void PHEventHandler::multitouchEnd(void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->state = PHEvent::multitouchEnd;
    for (set<PHView*>::iterator i = mtviews.begin(); i!=mtviews.end(); i++)
        (*i)->touchEvent(event);
    event->release();   
}

void PHEventHandler::registerViewForMultitouchEvents(PHView* v)
{
    mtviews.insert(v);
}

void PHEventHandler::unregisterViewForMultitouchEvents(PHView* v)
{
    mtviews.erase(v);
}

void PHEventHandler::removeView(PHView * view)
{
    unregisterViewForMultitouchEvents(view);
	for (map<void*,PHEvent*>::iterator i = events.begin(); i!= events.end(); i++)
	{
        PHEvent * e = i->second;
		if (e->_ownerView == view)
			e->_ownerView = NULL;
	}
}
