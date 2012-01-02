//
//  PHCinematicAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHCINEMATICANIMATOR_H
#define PHCINEMATICANIMATOR_H

#include "PHAnimator.h"

class PHCinematicAnimator;
class PHCinematicActor
{
protected:
    virtual void setCinematicPosition(const PHPoint &) = 0;
    virtual PHPoint cinematicPosition() = 0;
    virtual void setCinematicRotation(double) = 0;
    virtual double cinematicRotation() = 0;
    virtual void setCinematicScale(const PHSize &) = 0;
    virtual PHSize cinematicScale() = 0;
    virtual void setCinematicBgColor(const PHColor &) = 0;
    virtual PHColor cinematicBgColor() = 0;
    virtual void setCinematicCustomColor(const PHColor &);
    virtual PHColor cinematicCustomColor();
    virtual void setCinematicCustomValue(double);
    virtual double cinematicCustomValue();
    
    PHCinematicAnimator * _cinematicAnimator;
    set<PHCinematicAnimator *> _cinematicAnimators;
    friend class PHCinematicAnimator;
    
public:
    void addCinematicAnimation(PHCinematicAnimator * anim);
    void removeCinematicAnimation(PHCinematicAnimator * anim);
    void removeAllCinematicAnimations();
    void removeCinematicAnimationsWithTag(int tag);
    
    void beginCinematicAnimation(double duration);
    void beginCinematicAnimation(double duration, int type);
    void chainCinematicAnimation(double duration);
    void chainCinematicAnimation(double duration, int type);
    void commitCinematicAnimation();
    void dropCinematicAnimation();
    
    PHCinematicActor();
    ~PHCinematicActor();
};

class PHCinematicAnimator : public PHAnimator
{
public:
    PHCinematicAnimator();
    PHCinematicAnimator(PHAnimatorPool * pool);
    virtual ~PHCinematicAnimator();
    
    virtual void advanceAnimation(double elapsedTime);
    
    enum Functions
	{
		LinearFunction = 0,
		BounceFunction,
		FadeInFunction,
		FadeOutFunction,
		FadeInOutFunction,
		NUMFUNCTIONS
	};
    
    void setScaling(const PHSize & sz) { scale = sz; }
    const PHSize & scaling() { return scale; }
    void setMovement(const PHSize & sz) { move = sz; }
    const PHSize & movement() { return move; }
    void setRotation(double r) { rotate = r; }
    double rotation() { return rotate; }
    void setCustomValueDelta(double val) { customValue = val; }
    double customValueDelta() { return customValue; }
    void setBgColor(const PHColor & clr) { _bgColor = clr; }
    const PHColor & bgColor() { return _bgColor; }    
    void setCustomColor(const PHColor & clr) { _customColor = clr; }
    const PHColor & customColor() { return _customColor; }
    int timeFunction() { return function; }
    void setTimeFunction(int tf) { function = tf; }
    double duration() { return time; }
    double totalDuration() { return totalTime; }
    void setDuration(double d) { time = totalTime = d; }
    void setCallback(PHInvocation cb) { callback = cb; }
    
    void setActor(PHCinematicActor * a) { _actor = a; }
    PHCinematicActor * actor() { return _actor; }
    void setNextAnimation(PHCinematicAnimator * nx);
    PHCinematicAnimator * nextAnimation() { return next; }
    void invalidate();
    void skip() { advanceAnimation(time); }
    void completed() { completed(0); }
    void completed(double remaining);
    
    
private:
    PHCinematicActor * _actor;
    PHCinematicAnimator * next;
    
    PHSize scale;
    PHSize move;
    double rotate;
	PHColor _bgColor,_customColor;
    double customValue;
    int function;
    double time,totalTime;
    PHInvocation callback;
    
    double f(double time,int ftype);
};

#endif
