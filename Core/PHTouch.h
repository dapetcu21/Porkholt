/*
 *  PHTouch.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHTOUCH_H
#define PHTOUCH_H

#include "PHMain.h"

class PHEventHandler;
class PHView;

class PHTouch : public PHObject
{
public:
	enum states
	{
		touchDownState = 0,
		touchMovedState,
		touchUpState,
		touchCancelledState
	};
private:
	double _lastTime;
	double _time;
	PHView * _ownerView;
	PHPoint _location;
	PHPoint _lastLocation;
	int state;
	void * ud;
	friend class PHEventHandler;
	void updateLocation(const PHPoint & pnt, double time, int nstate);
	
public:
	
	PHTouch() : _ownerView(NULL) {};
	
	double time() { return _time; };
	double lastTime() { return _lastTime; };
	PHPoint location() { return _location; };
	PHPoint lastLocation() { return _lastLocation; };
	PHView * ownerView() { return _ownerView; };
	int phase() { return state; };
};

#endif