/*
 *  PHEventHandler.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHEventHandler.h"
#include "PHGameManager.h"
#include "PHEvent.h"
#include "PHView.h"

PHEvent * PHEventHandler::touchForUserData(void * ud, list<PHEvent*>::iterator & i)
{
	for (i = touches.begin(); i!= touches.end(); i++)
	{
		if ((*i)->ud == ud)
			return (*i);
	}
	return NULL;
}

void PHEventHandler::touchDown(PHPoint pnt,void * ud)
{
	PHEvent * touch = new PHEvent();
	touch->updateLocation(pnt, PHTime::getTime(), PHEvent::touchDown);
	touch->ud = ud;
	touches.push_back(touch);
	touch->_ownerView = gameManager->mainView()->pointerDeepFirst(touch);
}

void PHEventHandler::touchUp(PHPoint pnt, void * ud)
{
	list<PHEvent*>::iterator i;
	PHEvent * touch = touchForUserData(ud,i);
	if (!touch) return;
	touch->updateLocation(pnt, PHTime::getTime(), PHEvent::touchUp);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
	touches.erase(i);
	touch->release();
}

void PHEventHandler::touchMoved(PHPoint pnt, void * ud)
{
	list<PHEvent*>::iterator i;
	PHEvent * touch = touchForUserData(ud,i);
	if (!touch) return;
	touch->updateLocation(pnt, PHTime::getTime(), PHEvent::touchMoved);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
}

void PHEventHandler::touchCancelled(PHPoint pnt, void *ud)
{
	list<PHEvent*>::iterator i;
	PHEvent * touch = touchForUserData(ud,i);
	if (!touch) return;
	touch->updateLocation(pnt, PHTime::getTime(), PHEvent::touchCancelled);
	if (touch->_ownerView)
		touch->_ownerView->touchEvent(touch);
	touches.erase(i);
	touch->release();
}

void PHEventHandler::scrollWheel(PHPoint pnt, PHPoint delta, void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->_delta = delta;
    event->_location = pnt;
    event->state = PHEvent::scrollWheel;
    event->_ownerView = gameManager->mainView()->pointerDeepFirst(event);
    event->release();
}

void PHEventHandler::pinchZoom(PHPoint pnt, double zoom, void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->_zoom = zoom;
    event->_location = pnt;
    event->state = PHEvent::pinchZoom;
    event->_ownerView = gameManager->mainView()->pointerDeepFirst(event);
    event->release();
}

void PHEventHandler::pinchRotate(PHPoint pnt, double rotation, void *ud)
{
    PHEvent * event = new PHEvent;
    event->ud = ud;
    event->_rotation = rotation;
    event->_location = pnt;
    event->state = PHEvent::pinchRotate;
    event->_ownerView = gameManager->mainView()->pointerDeepFirst(event);
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
	for (list<PHEvent*>::iterator i = touches.begin(); i!= touches.end(); i++)
	{
		if ((*i)->_ownerView == view)
			(*i)->_ownerView = NULL;
	}
}