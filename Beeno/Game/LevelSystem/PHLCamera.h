/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLCAMERA_H
#define PHLCAMERA_H


#include "PHLObject.h"

class b2World;

class PHLCamera : public PHLObject
{
protected:
	PHRect sz;
    bool follow, strict;
    int strictcount;
    float sstrict;
public:
	PHLCamera();
	virtual ~PHLCamera();
	
	PHRect bounds() { return sz; };
    float width() { return sz.width; }
    float height() { return sz.height; }
	
	virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
	virtual void loadView();
	
	void updateCamera(PHPoint pnt, float elapsed);
    
    void setFollowsPlayer(bool f) { follow = f; }
    bool followsPlayer() { return follow; }
    
    static void registerLuaInterface(lua_State * L);
    
    void setStrictTracking(bool f) { strict = f; }
    bool strictTracking() { return strict; }
    
    void setSmoothStrictTracking(bool f) { if (f) { strict = f; sstrict = 0; } else { sstrict = 1.0f; } }
    
    void incrementStrictTracking() { strictcount++; setSmoothStrictTracking(strictcount); } 
    void decrementStrictTracking() { strictcount--; setSmoothStrictTracking(strictcount); }
};

#endif
