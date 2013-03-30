/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLANIMATION_H
#define PHLANIMATION_H

#include <Porkholt/Core/PHMain.h>

class PHLAnimation : public PHObject
{
private:
    float time;
    PHPoint move,rotateCenter,force,impulse,forceapp,velocity;
    bool useRotateCenter,objCoord;
    float rotate,angularImpulse,corrForce;
    float elapsed;
    float position; //f(elapsed)
    float braking;
    bool valid,skipped,statica,odyn;
    
    lua_State * L;
    int function;
    
    PHLAnimation * next;
    
    PHInvocation invocation;
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
    
    void setBrakeForce(float force) { braking = force; }
    float brakeForce() { return braking; }
    void setMovement(PHPoint movement) { move = movement; }
    void setForce(PHPoint frc, bool objectCoordinates) { force = frc;  objCoord = objectCoordinates;}
    void setImpulse(PHPoint frc, bool objectCoordinates) { impulse = frc;  objCoord = objectCoordinates;}
    void setAngularImpulse(float ai) { angularImpulse = ai; }
    void setVelocity(PHPoint vel, float correctorForce) { velocity = vel; corrForce = correctorForce; }
    void setForceApplicationPoint(PHPoint app) { forceapp = app; }
    void setRotation(float rot)
    {
        rotate = rot;
        useRotateCenter = false;
    }
    void setRotationAround(float rot, PHPoint rc)
    {
        rotate = rot;
        rotateCenter = rc;
        useRotateCenter = true;
    }
    void setTime(float tm) { time = tm; }
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
    
    void setCallback(const PHInvocation & inv) { invocation = inv; }
    void setCallbackOnInvalidate(bool ci) { invalidateCallback = ci; }
    void animationFinished();
    
    void loadFromLua(lua_State * L);
    static void registerLuaInterface(lua_State * L);
    
    float f(float x);
    
    virtual void animationStepped(float elapsed) {};
};

#endif
