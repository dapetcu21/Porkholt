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
    PHPoint move,rotateCenter,force,impulse,forceapp,velocity;
    bool useRotateCenter,objCoord;
    double rotate,angularImpulse,corrForce;
    double elapsed;
    double position; //f(elapsed)
    double breaking;
    bool valid,skipped,statica,odyn;
    
    lua_State * L;
    int function;
    
    PHLAnimation * next;
    
    PHObject * cbTarget;
    PHCallback cbFunction;
    void * cbUd;
    bool invalidateCallback;
    
    friend class PHLObject;
public:
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
    
    PHLAnimation();
    virtual ~PHLAnimation();
    
    void setBreak(double force) { breaking = force; }
    double breakForce() { return breaking; }
    void setMovement(PHPoint movement) { move = movement; }
    void setForce(PHPoint frc, bool objectCoordinates) { force = frc;  objCoord = objectCoordinates;}
    void setImpulse(PHPoint frc, bool objectCoordinates) { impulse = frc;  objCoord = objectCoordinates;}
    void setAngularImpulse(double ai) { angularImpulse = ai; }
    void setVelocity(PHPoint vel, double correctorForce) { velocity = vel; corrForce = correctorForce; }
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
    void setCurveFunction(int f) { function = f; }
    
    void setNextAnimation(PHLAnimation * anim) {
        if (anim) anim->retain();
        if (next) next->release();
        next = anim;
    }
    PHLAnimation * nextAnimation() { return next; }
    
    void setCallback(PHObject * target, PHCallback cb, void * ud)
    {
        cbTarget = target;
        cbFunction = cb;
        cbUd = ud;
    }
    void setCallbackOnInvalidate(bool ci) { invalidateCallback = ci; }
    void animationFinished();
    
    void loadFromLua(lua_State * L);
    static void registerLuaInterface(lua_State * L);
    
    double f(double x);
    
    virtual void animationStepped(double elapsed) {};
};

#endif