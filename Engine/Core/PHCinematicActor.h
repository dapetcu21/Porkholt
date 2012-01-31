//
//  PHCinematicActor.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/8/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHCINEMATICACTOR_H
#define PHCINEMATICACTOR_H

#include "PHMain.h"

class PHCinematicAnimator;
class PHGenericCinematicAnimator;
class PHCinematicActor
{
public:
    virtual void setCinematicPosition(const PHPoint &) = 0;
    virtual PHPoint cinematicPosition() = 0;
    virtual void setCinematicRotation(ph_float) = 0;
    virtual ph_float cinematicRotation() = 0;
    virtual void setCinematicScale(const PHSize &) = 0;
    virtual PHSize cinematicScale() = 0;
    virtual void setCinematicBgColor(const PHColor &) = 0;
    virtual PHColor cinematicBgColor() = 0;
    virtual void setCinematicCustomColor(const PHColor &);
    virtual PHColor cinematicCustomColor();
    virtual void setCinematicCustomValue(ph_float);
    virtual ph_float cinematicCustomValue();
    
protected:
    
    PHCinematicAnimator * _cinematicAnimator, * _rootAnimator;
    set<PHGenericCinematicAnimator *> _cinematicAnimators;
    friend class PHGenericCinematicAnimator;
    PHMutex * _cinematicMutex;
    
public:
    void addCinematicAnimation(PHGenericCinematicAnimator * anim);
    void removeCinematicAnimation(PHGenericCinematicAnimator * anim);
    void removeAllCinematicAnimations();
    void removeCinematicAnimationsWithTag(int tag);
    
    PHCinematicAnimator * cinematicAnimator() { return _cinematicAnimator; }
    void beginCinematicAnimation(ph_float duration);
    void beginCinematicAnimation(ph_float duration, int type);
    void chainCinematicAnimation(ph_float duration);
    void chainCinematicAnimation(ph_float duration, int type);
    void commitCinematicAnimation();
    void dropCinematicAnimation();
    
    void animateMove(const PHPoint & mv);
    void animateScale(const PHSize & mv);
    void animateRotate(ph_float rot);
    void animateBgColor(const PHColor & clr);
    void animateCustomColor(const PHColor & clr);
    void animateCustomValue(ph_float val);
    void animationCallback(const PHInvocation & inv);
    void animationTag(int tag);
    
    PHCinematicActor();
    ~PHCinematicActor();
};

#endif
