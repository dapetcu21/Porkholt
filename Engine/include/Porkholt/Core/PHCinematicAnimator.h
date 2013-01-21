/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCINEMATICANIMATOR_H
#define PHCINEMATICANIMATOR_H

//#define PH_SLOW_CINEMATIC_ANIMATIONS

#include <Porkholt/Core/PHGenericCinematicAnimator.h>
#include <Porkholt/Core/PHAnimationField.h>

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
    
    int skipsFirstFrames() { return skipFirst; }
    void setSkipsFirstFrames(int s) { skipFirst = s; }
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

    void addAnimationField(const PHAnimationField & anim);
    
private:
    PHCinematicAnimator * next;

    vector<PHAnimationField> newFields;
    vector<PHAnimationField> fields;
    
    int function;
    ph_float time,totalTime;
    int skipFirst;
    PHInvocation callback;
    
    ph_float f(ph_float time,int ftype);
};

#endif
