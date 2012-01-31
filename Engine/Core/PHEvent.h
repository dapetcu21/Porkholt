/*
 *  PHEvent.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHEvent_H
#define PHEvent_H

#include "PHMain.h"

class PHEventHandler;
class PHView;

class PHEvent : public PHObject
{
public:
	enum states
	{
		touchDown = 0,
		touchMoved,
		touchUp,
		touchCancelled,
        scrollWheel,
        pinchZoom,
        pinchRotate,
        multitouchBegin,
        multitouchEnd
	};
private:
	ph_float _lastTime;
	ph_float _time;
	PHView * _ownerView;
	PHPoint _location;
	PHPoint _lastLocation;
    ph_float _zoom;
    PHPoint _delta;
    ph_float _rotation;
	int state;
	void * ud;
    PHObject * _sender;
    bool _handled;
	friend class PHEventHandler;
	void updateLocation(const PHPoint & pnt, ph_float time, int nstate);
public:
	
	PHEvent() : _ownerView(NULL), _sender(NULL), _handled(false) {};
	
	ph_float time() { return _time; };
	ph_float lastTime() { return _lastTime; };
	PHPoint location() { return _location; };
	PHPoint lastLocation() { return _lastLocation; };
	PHView * ownerView() { return _ownerView; };
    PHPoint delta() { return _delta; }
    ph_float deltaX() { return _delta.x; }
    ph_float deltaY() { return _delta.y; }
    ph_float rotation() { return _rotation; }
    ph_float zoom() { return _zoom; }
	int type() { return state; };
    void * userData() { return ud; }
    PHObject * sender() { return _sender; }
    void setSender(PHObject * s) { _sender = s; }
    void setOwnerView(PHView * ownerView) { _ownerView = ownerView; }
    bool handled() { return _handled; }
    void setHandled(bool h) { _handled = h; }
};

#endif