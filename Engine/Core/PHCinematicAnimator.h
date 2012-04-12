//
//  PHCinematicAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHCINEMATICANIMATOR_H
#define PHCINEMATICANIMATOR_H

//#define PH_SLOW_CINEMATIC_ANIMATIONS

#include "PHGenericCinematicAnimator.h"

class PHCinematicActor;
class PHCinematicAnimator : public PHGenericCinematicAnimator
{
public:
    PHCinematicAnimator();
    PHCinematicAnimator(PHAnimatorPool * pool);
    virtual ~PHCinematicAnimator();
    
    virtual void advanceAnimation(ph_float elapsedTime);
    
    enum Functions
	{
		LinearFunction = 0,
		BounceFunction,
		FadeInFunction,
		FadeOutFunction,
		FadeInOutFunction,
		NUMFUNCTIONS
	};
    
    bool skipsFirstFrame() { return skipFirst; }
    void setSkipsFirstFrame(bool s) { skipFirst = s; }
    void setScaling(const PHSize & sz) { scale = sz; }
    const PHSize & scaling() { return scale; }
    void setMovement(const PHSize & sz) { move = sz; }
    const PHSize & movement() { return move; }
    void setRotation(ph_float r) { rotate = r; }
    ph_float rotation() { return rotate; }
    void setCustomValueDelta(ph_float val) { customValue = val; }
    ph_float customValueDelta() { return customValue; }
    void setBgColor(const PHColor & clr) { _bgColor = clr; }
    const PHColor & bgColor() { return _bgColor; }    
    void setCustomColor(const PHColor & clr) { _customColor = clr; }
    const PHColor & customColor() { return _customColor; }
    int timeFunction() { return function; }
    void setTimeFunction(int tf) { function = tf; }
    ph_float duration() { return time; }
    ph_float totalDuration() { return totalTime; }
    void setDuration(ph_float d) { time = totalTime = d
#ifdef PH_SLOW_CINEMATIC_ANIMATIONS
        * 5;
#endif
        ; }
    void setCallback(const PHInvocation & cb) { callback = cb; }
    
    void setNextAnimation(PHCinematicAnimator * nx);
    PHCinematicAnimator * nextAnimation() { return next; }
    void invalidate();
    void skip() { advanceAnimation(time); }
    void completed() { completed(0); }
    void completed(ph_float remaining);
    
    
private:
    PHCinematicAnimator * next;
    
    PHSize scale;
    PHSize move;
    ph_float rotate;
	PHColor _bgColor,_customColor;
    ph_float customValue;
    int function;
    ph_float time,totalTime;
    bool skipFirst;
    PHInvocation callback;
    
    ph_float f(ph_float time,int ftype);
};

#endif
