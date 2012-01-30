//
//  PHGenericCinematicAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 1/8/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#ifndef PHGENERICCINEMATICANIMATOR_H
#define PHGENERICCINEMATICANIMATOR_H

#include "PHAnimator.h"

class PHCinematicActor;
class PHGenericCinematicAnimator : public PHAnimator
{
protected:
    PHCinematicActor * _actor;
public:
    void setActor(PHCinematicActor * a) { _actor = a; }
    PHCinematicActor * actor() { return _actor; }
    
    PHGenericCinematicAnimator() : _actor(NULL) {};
    PHGenericCinematicAnimator(PHAnimatorPool * pool) : _actor(NULL) {};
    virtual ~PHGenericCinematicAnimator() {}
};

#endif
