/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHEvent_H
#define PHEvent_H

#include <Porkholt/Core/PHMain.h>

class PHEventHandler;
class PHDrawable;
class PHDrawableCoordinates;

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
	PHDrawable * _owner;
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
    
    PHDrawableCoordinates * coord, * lastCoord;
public:
	
	PHEvent();
    PHEvent(PHEvent * o);
	~PHEvent();
	
	ph_float time() { return _time; };
    void setTime(ph_float t) { _time = t; }
	ph_float lastTime() { return _lastTime; };
    void setLastTime(ph_float t) { _lastTime = t; }
	PHPoint location() { return _location; };
    void setLocation(const PHPoint & p) { _location = p; }
	PHPoint lastLocation() { return _lastLocation; };
	void setLastLocation(const PHPoint & p) { _lastLocation = p; };
    PHPoint delta() { return _delta; }
    void setDelta(PHPoint & d) { _delta = d; }
    ph_float deltaX() { return _delta.x; }
    ph_float deltaY() { return _delta.y; }
    ph_float rotation() { return _rotation; }
    void setRotation(ph_float r) { _rotation = r; }
    ph_float zoom() { return _zoom; }
    void setZoom(ph_float z) { _zoom = z; }
	int type() { return state; };
    void setType(int t) { state = t; }
    void setUserData(void * u) { ud = u; }
    void * userData() { return ud; }
    PHObject * sender() { return _sender; }
    void setSender(PHObject * s) { _sender = s; }
    PHDrawable * owner() { return _owner; }
    void setOwner(PHDrawable * owner) { _owner= owner; }
    bool handled() { return _handled; }
    void setHandled(bool h) { _handled = h; }

    PHDrawableCoordinates * drawableLocation();
    PHDrawableCoordinates * lastDrawableLocation();
};

#endif
