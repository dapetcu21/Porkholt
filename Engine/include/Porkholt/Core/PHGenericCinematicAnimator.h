/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGENERICCINEMATICANIMATOR_H
#define PHGENERICCINEMATICANIMATOR_H

#include <Porkholt/Core/PHAnimator.h>

class PHCinematicActor;
class PHGenericCinematicAnimator : public PHAnimator
{
protected:
    PHCinematicActor * _actor;
public:
    void setActor(PHCinematicActor * a) { _actor = a; }
    PHCinematicActor * actor() { return _actor; }
    
    PHGenericCinematicAnimator() : _actor(NULL) {};
    PHGenericCinematicAnimator(PHAnimatorPool * pool) : PHAnimator(pool), _actor(NULL) {};
    virtual ~PHGenericCinematicAnimator() {}
};

#endif
