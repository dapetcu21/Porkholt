/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHCinematicActor.h>
#include <Porkholt/Core/PHCinematicAnimator.h>
#include <Porkholt/Core/PHMutex.h>
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

void PHCinematicActor::animateField(const PHAnimationField & field)
{
    if (_cinematicAnimator)
        _cinematicAnimator->addAnimationField(field);
    else
    {
        beginCinematicAnimation(0.5f);
        _cinematicAnimator->addAnimationField(field);
        commitCinematicAnimation();
    }
}

void PHCinematicActor::animationCallback(const PHInvocation & inv)
{
    _cinematicAnimator->setCallback(inv);
}

void PHCinematicActor::animationTag(size_t tag)
{
    _cinematicAnimator->setTag(tag);
}

void PHCinematicActor::animationSkipFirstFrames(int n)
{
    _cinematicAnimator->setSkipsFirstFrames(n);
}

void PHCinematicActor::actorAttachedToGameManager(PHGameManager * gameman)
{
    _gm = gameman;
    for (set<PHGenericCinematicAnimator *>::iterator i = _cinematicAnimators.begin(); i!=_cinematicAnimators.end(); i++)
        if (!((*i)->animatorPool()))
            (*i)->setAnimatorPool(_gm->animatorPool());
}

void PHCinematicActor::setAnimationFieldF(int field, ph_float v)
{
}

void PHCinematicActor::setAnimationFieldV2(int field, const PHVector2 & v)
{
}

void PHCinematicActor::setAnimationFieldV3(int field, const PHVector3 & v)
{
}

void PHCinematicActor::setAnimationFieldC(int field, const PHColor & v)
{
}

void PHCinematicActor::setAnimationFieldQ(int field, const PHQuaternion & v)
{
}

ph_float PHCinematicActor::getAnimationFieldF(int field)
{
    return 0;
}

PHVector2 PHCinematicActor::getAnimationFieldV2(int field)
{
    return PHOriginPoint;
}

PHVector3 PHCinematicActor::getAnimationFieldV3(int field)
{
    return PH3DOriginPoint;
}

PHColor PHCinematicActor::getAnimationFieldC(int field)
{
    return PHClearColor;
}

PHQuaternion PHCinematicActor::getAnimationFieldQ(int field)
{
    return PHIdentityQuaternion;
}


