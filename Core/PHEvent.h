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
	double _lastTime;
	double _time;
	PHView * _ownerView;
	PHPoint _location;
	PHPoint _lastLocation;
    double _zoom;
    PHPoint _delta;
    double _rotation;
	int state;
	void * ud;
	friend class PHEventHandler;
	void updateLocation(const PHPoint & pnt, double time, int nstate);
public:
	
	PHEvent() : _ownerView(NULL) {};
	
	double time() { return _time; };
	double lastTime() { return _lastTime; };
	PHPoint location() { return _location; };
	PHPoint lastLocation() { return _lastLocation; };
	PHView * ownerView() { return _ownerView; };
    PHPoint delta() { return _delta; }
    double deltaX() { return _delta.x; }
    double deltaY() { return _delta.y; }
    double rotation() { return _rotation; }
    double zoom() { return _zoom; }
	int type() { return state; };
};

#endif