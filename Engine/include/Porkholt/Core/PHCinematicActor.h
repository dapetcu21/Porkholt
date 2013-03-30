/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHCINEMATICACTOR_H
#define PHCINEMATICACTOR_H

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHAnimationField.h>

class PHCinematicAnimator;
class PHGenericCinematicAnimator;
class PHGameManager;

class PHCinematicActor
{
public:
    enum fields
    {
        fieldMove,
        fieldScale,
        fieldRotate,
        fieldBgColor,
        fieldColor,
        fieldDirection,
        fieldVelocity,
        fieldGain,
        fieldPitch,
    };
    enum types
    {
        tNone,
        tFloat,
        tVec2,
        tVec3,
        tColor,
        tQuat
    };

    virtual void setAnimationFieldF(int field, float v);
    virtual void setAnimationFieldV2(int field, const PHVector2 & v);
    virtual void setAnimationFieldV3(int field, const PHVector3 & v);
    virtual void setAnimationFieldC(int field, const PHColor & v);
    virtual void setAnimationFieldQ(int field, const PHQuaternion & v);
    virtual float getAnimationFieldF(int field);
    virtual PHVector2 getAnimationFieldV2(int field);
    virtual PHVector3 getAnimationFieldV3(int field);
    virtual PHColor getAnimationFieldC(int field);
    virtual PHQuaternion getAnimationFieldQ(int field);

protected:
    
    PHCinematicAnimator * _cinematicAnimator, * _rootAnimator;
    set<PHGenericCinematicAnimator *> _cinematicAnimators;
    friend class PHGenericCinematicAnimator;
    PHMutex * _cinematicMutex;
    PHGameManager * _gm;

public:
    PHGameManager * actorGameManager() { return _gm; }

    void addCinematicAnimation(PHGenericCinematicAnimator * anim);
    void removeCinematicAnimation(PHGenericCinematicAnimator * anim);
    void removeAllCinematicAnimations();
    void removeCinematicAnimationsWithTag(size_t tag);
    
    PHCinematicAnimator * cinematicAnimator() { return _cinematicAnimator; }
    void beginCinematicAnimation(float duration);
    void beginCinematicAnimation(float duration, int type);
    void chainCinematicAnimation(float duration);
    void chainCinematicAnimation(float duration, int type);
    void commitCinematicAnimation();
    void dropCinematicAnimation();
    
    void animateMove(const PHPoint & mv) { animateField(PHAnimationField(fieldMove, mv, true)); }
    void animateMove(const PH3DPoint & mv) { animateField(PHAnimationField(fieldMove, mv, true)); }
    void animateScale(const PHSize & mv) { animateField(PHAnimationField(fieldScale, mv, true, true)); }
    void animateScale(const PH3DSize & mv) { animateField(PHAnimationField(fieldScale, mv, true, true)); }
    void animateRotate(float rot) { animateField(PHAnimationField(fieldRotate, true)); }
    void animateRotate(const PHQuaternion & q) { animateField(PHAnimationField(fieldRotate, q, true)); }

    void animateField(const PHAnimationField & field);

    void animationCallback(const PHInvocation & inv);
    void animationSkipFirstFrames(int f);
    void animationTag(size_t tag);
    
    PHCinematicActor();
    ~PHCinematicActor();
    
protected:
    void actorAttachedToGameManager(PHGameManager * gm);
 
};

#endif
