//
//  PHLAnimation.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/4/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLANIMATION_H
#define PHLANIMATION_H

#include "PHMain.h"

class PHLAnimation : public PHObject
{
private:
    double time;
    PHPoint move,rotateCenter,force,forceapp;
    bool useRotateCenter,objCoord;
    double rotate;
    double elapsed;
    double position; //f(elapsed)
    bool valid,skipped,statica,odyn;
    
    enum Functions
	{
		LinearFunction = 0,
		BounceFunction,
		FadeInFunction,
		FadeOutFunction,
		FadeInOutFunction,
        ConstantFunction, 
        LuaFunction,
		NUMFUNCTIONS
	};
    
    lua_State * L;
    int function;
    
    PHLAnimation * next;
    
    friend class PHLObject;
public:
    PHLAnimation();
    ~PHLAnimation();
    
    void setMovement(PHPoint movement) { move = movement; }
    void setForce(PHPoint frc, bool objectCoordinates) { force = frc;  objCoord = objectCoordinates;}
    void setForceApplicationPoint(PHPoint app) { forceapp = app; }
    void setRotation(double rot)
    {
        rotate = rot;
        useRotateCenter = false;
    }
    void setRotationAround(double rot, PHPoint rc)
    {
        rotate = rot;
        rotateCenter = rc;
        useRotateCenter = true;
    }
    void setTime(double tm) { time = tm; }
    bool isValid() { return valid; }
    void invalidate() { valid = false; }
    bool isSkipped() { return skipped; }
    void skip() { skipped = true; }
    void skipChain();
    void invalidateChain();
    void setDisableDynamics(bool d) { statica = d; };
    bool isDisablingDynamics() { return statica; }
    
    void setNextAnimation(PHLAnimation * anim) {
        if (anim) anim->retain();
        if (next) next->release();
        next = anim;
    }
    PHLAnimation * nextAnimation() { return next; }
    
    void loadFromLua(lua_State * L);
    static void registerLuaInterface(lua_State * L);
    
    double f(double x);
};

#endif