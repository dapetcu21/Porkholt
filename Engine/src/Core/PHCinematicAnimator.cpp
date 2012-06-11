/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHCinematicAnimator.h>
#include <Porkholt/Core/PHCinematicActor.h>

#define ANIMATOR_INIT next(NULL), scale(1,1), move(0,0), rotate(0), _bgColor(PHInvalidColor), _customColor(PHInvalidColor), customValue(0), function(LinearFunction), time(0), totalTime(0), skipFirst(false)

PHCinematicAnimator::PHCinematicAnimator() : PHGenericCinematicAnimator(), ANIMATOR_INIT
{
}

PHCinematicAnimator::PHCinematicAnimator(PHAnimatorPool * pool) : PHGenericCinematicAnimator(pool), ANIMATOR_INIT
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
    if (_actor)
        _actor->removeCinematicAnimation(this);
    _actor = NULL;
}

void PHCinematicAnimator::completed(ph_float remaining)
{
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

inline ph_float circleEq(ph_float x,ph_float mx,ph_float my,ph_float r)
{
	return sqrt(r*r-(x-mx)*(x-mx))+my;
}
#define bounceL 0.5f
ph_float PHCinematicAnimator::f(ph_float time,int ftype)
{
	if (ftype==LinearFunction)
		return time;
	if (ftype==BounceFunction)
	{
		if (time<1-bounceL)
		{
			return time/(1-bounceL);
		} else {
			ph_float m = (bounceL-1);
			ph_float n = 1-m*(1-bounceL);
			ph_float yy = m*(1-bounceL/2)+n;
			ph_float r = sqrt(bounceL*bounceL/4+(1-yy)*(1-yy));
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
		return (ph_float)(sin(M_PI*(time-0.5f))/2+0.5);
	}
	return time; //revert to linear
}

void PHCinematicAnimator::advanceAnimation(ph_float elapsedTime)
{
    if (!_actor)
        return;
    if (skipFirst)
    {
        skipFirst = false;
        return;
    }
    ph_float tm = elapsedTime;
    ph_float lastTime = time;
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
        ph_float lastRatio = f(1.0f - (lastTime/totalTime),function);
        ph_float ratio = f(1.0f - (time/totalTime),function);
        ph_float delta = ratio - lastRatio;
        if (move.x || move.y)
            _actor->setCinematicPosition(_actor->cinematicPosition()+(move * delta));
        if (rotate)
            _actor->setCinematicRotation(_actor->cinematicRotation()+(rotate * delta));
        if (customValue)
            _actor->setCinematicCustomValue(_actor->cinematicCustomValue()+(customValue * delta));
        if (scale.x != 1 || scale.y != 1)
        {
            PHSize lastFactor = (scale-PHSize(1,1))*lastRatio+PHSize(1,1);
            PHSize factor = (scale-PHSize(1,1))*ratio+PHSize(1,1);
            PHSize orig = _actor->cinematicScale();
            _actor->setCinematicScale(PHSize(orig.x/lastFactor.x*factor.x,orig.y/lastFactor.y*factor.y));
        }
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
    if (time<=FLT_EPSILON)
    {
        completed(elapsedTime-tm);
    }
}
