/*
 *  PHLCamera.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHLCAMERA_H
#define PHLCAMERA_H


#include "PHLObject.h"

class b2World;

class PHLCamera : public PHLObject
{
protected:
	PHRect sz;
public:
	PHLCamera();
	virtual ~PHLCamera();
	
	PHRect bounds() { return sz; };
    double width() { return sz.width; }
    double height() { return sz.height; }
	
	virtual void loadFromLua(lua_State * L, const string & root,b2World * world);
	virtual void loadView();
	
	void updateCamera(PHPoint pnt);
};

#endif
