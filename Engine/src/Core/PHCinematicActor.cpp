/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHCinematicActor.h>
#include <Porkholt/Core/PHCinematicAnimator.h>
#include <Porkholt/Core/PHGameManager.h>

void PHCinematicActor::addCinematicAnimation(PHGenericCinematicAnimator * anim)
{
    if (!anim) return;
    _cinematicMutex->lock();
    if (_gm && !(anim->animatorPool()))
        anim->setAnimatorPool(_gm->animatorPool());
    if (_cinematicAnimators.insert(anim).second)
        anim->retain();
    anim->setActor(this);
    _cinematicMutex->unlock();
}

void PHCinematicActor::removeCinematicAnimation(PHGenericCinematicAnimator * anim)
{
    if (!anim) return;
    _cinematicMutex->lock();
    anim->setActor(NULL);
    if (_cinematicAnimators.erase(anim))
        anim->release();
    _cinematicMutex->unlock();
}

void PHCinematicActor::removeAllCinematicAnimations()
{
    _cinematicMutex->lock();
    for (set<PHGenericCinematicAnimator*>::iterator i = _cinematicAnimators.begin(); i!=_cinematicAnimators.end(); i++)
    {
        (*i)->setActor(NULL);
        (*i)->release();
    }
    _cinematicAnimators.clear();
    _cinematicMutex->unlock();
}

void PHCinematicActor::removeCinematicAnimationsWithTag(size_t tag)
{
    list<PHGenericCinematicAnimator*> delList;
    _cinematicMutex->lock();
    for (set<PHGenericCinematicAnimator*>::iterator i = _cinematicAnimators.begin(); i!=_cinematicAnimators.end(); i++)
    {
        PHGenericCinematicAnimator * anim = (*i);
        if (anim->tag() == tag)
        {
            delList.push_back(anim);
        }
    }
    for (list<PHGenericCinematicAnimator*>::iterator i = delList.begin(); i!=delList.end(); i++)
    {
        PHGenericCinematicAnimator * anim = (*i);
        _cinematicAnimators.erase(anim);
        anim->setActor(NULL);
        anim->release();
    }
    _cinematicMutex->unlock();
}

void PHCinematicActor::setCinematicCustomColor(const PHColor &) {}
PHColor PHCinematicActor::cinematicCustomColor() { return PHInvalidColor; }
void PHCinematicActor::setCinematicCustomValue(ph_float) {}
ph_float PHCinematicActor::cinematicCustomValue() { return 0; }

PHCinematicActor::PHCinematicActor() : _cinematicAnimator(NULL) , _rootAnimator(NULL), _cinematicMutex(new PHMutex), _gm(NULL) {};
PHCinematicActor::~PHCinematicActor()
{
    dropCinematicAnimation();
    removeAllCinematicAnimations();
    _cinematicMutex->release();
}

void PHCinematicActor::beginCinematicAnimation(ph_float duration)
{
    beginCinematicAnimation(duration, PHCinematicAnimator::LinearFunction);
}

void PHCinematicActor::chainCinematicAnimation(ph_float duration)
{
    chainCinematicAnimation(duration, PHCinematicAnimator::LinearFunction);
}

void PHCinematicActor::beginCinematicAnimation(ph_float duration, int type)
{
    if (_cinematicAnimator)
        dropCinematicAnimation();
    _cinematicAnimator = _rootAnimator = new PHCinematicAnimator;
    _cinematicAnimator->setDuration(duration);
    _cinematicAnimator->setTimeFunction(type);
}

void PHCinematicActor::chainCinematicAnimation(ph_float duration, int type)
{
    if (!_cinematicAnimator)
    {
        beginCinematicAnimation(duration,type);
    }
    PHCinematicAnimator * ca = new PHCinematicAnimator;
    ca->setDuration(duration);
    ca->setTimeFunction(type);
    _cinematicAnimator->setNextAnimation(ca);
    ca->release();
    _cinematicAnimator = ca;
}

void PHCinematicActor::commitCinematicAnimation()
{
    addCinematicAnimation(_rootAnimator);
    dropCinematicAnimation();
}

void PHCinematicActor::dropCinematicAnimation()
{
    if (_rootAnimator)
    {
        _rootAnimator->release();
    }
    _rootAnimator = _cinematicAnimator = NULL;
}

void PHCinematicActor::animateMove(const PHPoint & mv)
{
    _cinematicAnimator->setMovement(_cinematicAnimator->movement()+mv);
}

void PHCinematicActor::animateScale(const PHSize & mv)
{
    PHSize s = _cinematicAnimator->scaling();
    _cinematicAnimator->setScaling(PHSize(mv.x*s.x,mv.y*s.y));
}

void PHCinematicActor::animateRotate(ph_float rot)
{
    _cinematicAnimator->setRotation(_cinematicAnimator->rotation()+rot);
}

void PHCinematicActor::animateBgColor(const PHColor & clr)
{
    _cinematicAnimator->setBgColor(clr);
}

void PHCinematicActor::animateCustomColor(const PHColor & clr)
{
    _cinematicAnimator->setCustomColor(clr);
}

void PHCinematicActor::animationCallback(const PHInvocation & inv)
{
    _cinematicAnimator->setCallback(inv);
}

void PHCinematicActor::animationTag(size_t tag)
{
    _cinematicAnimator->setTag(tag);
}

void PHCinematicActor::animationSkipFirstFrame()
{
    _cinematicAnimator->setSkipsFirstFrame(true);
}

void PHCinematicActor::animateCustomValue(ph_float val)
{
    _cinematicAnimator->setCustomValueDelta(_cinematicAnimator->customValueDelta()+val);
}

void PHCinematicActor::actorAttachedToGameManager(PHGameManager * gameman)
{
    _gm = gameman;
    for (set<PHGenericCinematicAnimator *>::iterator i = _cinematicAnimators.begin(); i!=_cinematicAnimators.end(); i++)
        if (!((*i)->animatorPool()))
            (*i)->setAnimatorPool(_gm->animatorPool());
}
