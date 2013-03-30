/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHCinematicAnimator.h>
#include <Porkholt/Core/PHCinematicActor.h>

#define ANIMATOR_INIT next(NULL), function(LinearFunction), time(0), totalTime(0), skipFirst(false)

PHCinematicAnimator::PHCinematicAnimator() : PHGenericCinematicAnimator(), ANIMATOR_INIT
{
}

PHCinematicAnimator::PHCinematicAnimator(PHAnimatorPool * pool) : PHGenericCinematicAnimator(pool), ANIMATOR_INIT
{
}

PHCinematicAnimator::~PHCinematicAnimator()
{
}

void PHCinematicAnimator::addAnimationField(const PHAnimationField & field)
{
    newFields.push_back(field);
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

void PHCinematicAnimator::completed(float remaining)
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

inline float circleEq(float x,float mx,float my,float r)
{
	return sqrt(r*r-(x-mx)*(x-mx))+my;
}
#define bounceL 0.5f
float PHCinematicAnimator::f(float time,int ftype)
{
	if (ftype==LinearFunction)
		return time;
	if (ftype==BounceFunction)
	{
		if (time<1-bounceL)
		{
			return time/(1-bounceL);
		} else {
			float m = (bounceL-1);
			float n = 1-m*(1-bounceL);
			float yy = m*(1-bounceL/2)+n;
			float r = sqrt(bounceL*bounceL/4+(1-yy)*(1-yy));
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
		return (float)(sin(M_PI*(time-0.5f))/2+0.5);
	}
	return time; //revert to linear
}

void PHCinematicAnimator::advanceAnimation(float elapsedTime)
{
    if (!_actor)
    {
        setAnimatorPool(NULL);
        return;
    }
    if (skipFirst)
    {
        skipFirst--;
        return;
    }
    float tm = elapsedTime;
    float lastTime = time;
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
        float lastRatio = f(1.0f - (lastTime/totalTime),function);
        float ratio = f(1.0f - (time/totalTime),function);

        if (newFields.size())
        {
            fields.reserve(fields.size() + newFields.size());
            for (vector<PHAnimationField>::iterator i = newFields.begin(); i != newFields.end(); i++)
            {
                i->initState(_actor);
                fields.push_back(*i);
            }
            newFields.clear();
        }

        for (vector<PHAnimationField>::iterator i = fields.begin(); i != fields.end(); i++)
            i->animate(_actor, lastRatio, ratio);
    }
    if (time<=FLT_EPSILON)
    {
        completed(elapsedTime-tm);
    }
}
