//
//  PHCinematicAnimator.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHCinematicAnimator.h"

#define ANIMATOR_INIT _actor(NULL), next(NULL), scale(PHSize(1,1)), move(PHSize(0,0)), rotate(0), _bgColor(PHInvalidColor), _customColor(PHInvalidColor), customValue(0), time(0), totalTime(0), function(LinearFunction)

PHCinematicAnimator::PHCinematicAnimator() : PHAnimator(), ANIMATOR_INIT
{
    
}

PHCinematicAnimator::PHCinematicAnimator(PHAnimatorPool * pool) : PHAnimator(pool), ANIMATOR_INIT
{
    
}

PHCinematicAnimator::~PHCinematicAnimator()
{
    
}

void PHCinematicAnimator::setNextAnimation(PHCinematicAnimator * nx)
{
    if (nx)
    {
        nx->retain();
        nx->setAnimatorPool(NULL);
    }
    if (next)
        next->release();
    next = nx;
}

void PHCinematicAnimator::invalidate()
{
    PHLog("invalidate");
    if (_actor)
        _actor->removeCinematicAnimation(this);
}

void PHCinematicAnimator::completed(double remaining)
{
    PHLog("completed");
    callback.call((PHObject*)_actor);
    if (next && _actor)
    {
        next->setAnimatorPool(animatorPool());
        _actor->addCinematicAnimation(next);
        if (remaining)
            next->advanceAnimation(remaining);
    }
    invalidate();
}

inline double circleEq(double x,double mx,double my,double r)
{
	return sqrt(r*r-(x-mx)*(x-mx))+my;
}
#define bounceL 0.5f
double PHCinematicAnimator::f(double time,int ftype)
{
	if (ftype==LinearFunction)
		return time;
	if (ftype==BounceFunction)
	{
		if (time<1-bounceL)
		{
			return time/(1-bounceL);
		} else {
			double m = (bounceL-1);
			double n = 1-m*(1-bounceL);
			double yy = m*(1-bounceL/2)+n;
			double r = sqrt(bounceL*bounceL/4+(1-yy)*(1-yy));
			return circleEq(time, 1-bounceL/2, yy, r);
		}
	}
	if (ftype==FadeInFunction)
	{
		return time*time;
	}
	if (ftype==FadeOutFunction)
	{
		return -time*time+2*time;
	}
	if (ftype==FadeInOutFunction)
	{
		return sin(M_PI*(time-0.5f))/2+0.5;
	}
	return time; //revert to linear
}

void PHCinematicAnimator::advanceAnimation(double elapsedTime)
{
    if (!_actor)
        return;
    double tm = elapsedTime;
    double lastTime = time;
    if (elapsedTime >= time)
    {  
        tm = time;
        time = 0;
    } else 
    {
        time -= tm;
    }
    if (tm>0 && totalTime>0)
    {
        double lastRatio = f(1.0f - (lastTime/totalTime),function);
        double ratio = f(1.0f - (time/totalTime),function);
        if (move.x || move.y)
            _actor->setCinematicPosition(_actor->cinematicPosition()+(move*ratio - move*lastRatio));
        if (rotate)
            _actor->setCinematicRotation(_actor->cinematicRotation()+(rotate*ratio - rotate*lastRatio));
        if (customValue)
            _actor->setCinematicCustomValue(_actor->cinematicCustomValue()+(customValue*ratio - customValue*lastRatio));
        if (_bgColor.a>=0)
        {
            PHColor clr,trg,crr;
            crr = _actor->cinematicBgColor();
            trg = _bgColor;
            if (trg.a>0)
            {
                if (crr.a>0)
                {
                    clr.r = ((lastRatio==1)?0:((crr.r-trg.r)/(1-lastRatio)))*(1-ratio)+trg.r;
                    clr.g = ((lastRatio==1)?0:((crr.g-trg.g)/(1-lastRatio)))*(1-ratio)+trg.g;
                    clr.b = ((lastRatio==1)?0:((crr.b-trg.b)/(1-lastRatio)))*(1-ratio)+trg.b;
                } else {
                    clr.r = trg.r;
                    clr.g = trg.g;
                    clr.b = trg.b;
                }
            } else {
                clr.r = crr.r;
                clr.g = crr.g;
                clr.b = crr.b;
            }
            clr.a = ((lastRatio==1)?0:((crr.a-trg.a)/(1-lastRatio)))*(1-ratio)+trg.a;
            _actor->setCinematicBgColor(clr);
        }
        if (_customColor.a>=0)
        {
            PHColor clr,trg,crr;
            crr = _actor->cinematicCustomColor();
            trg = _customColor;
            if (trg.a>0)
            {
                if (crr.a>0)
                {
                    clr.r = ((lastRatio==1)?0:((crr.r-trg.r)/(1-lastRatio)))*(1-ratio)+trg.r;
                    clr.g = ((lastRatio==1)?0:((crr.g-trg.g)/(1-lastRatio)))*(1-ratio)+trg.g;
                    clr.b = ((lastRatio==1)?0:((crr.b-trg.b)/(1-lastRatio)))*(1-ratio)+trg.b;
                } else {
                    clr.r = trg.r;
                    clr.g = trg.g;
                    clr.b = trg.b;
                }
            } else {
                clr.r = crr.r;
                clr.g = crr.g;
                clr.b = crr.b;
            }
            clr.a = ((lastRatio==1)?0:((crr.a-trg.a)/(1-lastRatio)))*(1-ratio)+trg.a;
            _actor->setCinematicCustomColor(clr);
        }
    }
    if (time<=0)
    {
        completed(elapsedTime-tm);
    }
}


void PHCinematicActor::addCinematicAnimation(PHCinematicAnimator * anim)
{
    if (!anim) return;
    if (_cinematicAnimators.insert(anim).second)
        anim->retain();
    anim->setActor(this);
}

void PHCinematicActor::removeCinematicAnimation(PHCinematicAnimator * anim)
{
    if (!anim) return;
    anim->setActor(NULL);
    if (_cinematicAnimators.erase(anim))
        anim->release();
}

void PHCinematicActor::removeAllCinematicAnimations()
{
    for (set<PHCinematicAnimator*>::iterator i = _cinematicAnimators.begin(); i!=_cinematicAnimators.end(); i++)
    {
        (*i)->setActor(NULL);
        (*i)->release();
    }
}

void PHCinematicActor::removeCinematicAnimationsWithTag(int tag)
{
    for (set<PHCinematicAnimator*>::iterator i = _cinematicAnimators.begin(); i!=_cinematicAnimators.end(); i++)
        if ((*i)->tag() == tag)
        {
            (*i)->setActor(NULL);
            (*i)->release();
        }    
}

void PHCinematicActor::setCinematicCustomColor(const PHColor &) {}
PHColor PHCinematicActor::cinematicCustomColor() { return PHInvalidColor; }
void PHCinematicActor::setCinematicCustomValue(double) {}
double PHCinematicActor::cinematicCustomValue() { return 0; }

PHCinematicActor::PHCinematicActor() : _cinematicAnimator(NULL) {};
PHCinematicActor::~PHCinematicActor()
{
    removeAllCinematicAnimations();
}