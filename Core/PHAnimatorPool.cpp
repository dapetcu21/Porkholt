//
//  PHAnimatorPool.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/30/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHAnimatorPool.h"
#include "PHImageAnimator.h"

PHAnimatorPool::PHAnimatorPool()
{
    
}

PHAnimatorPool::~PHAnimatorPool()
{
    for (set<PHImageAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        (*i)->setAnimatorPool(NULL);
    }
}

void PHAnimatorPool::insertAnimator(PHImageAnimator * a)
{
    animators.insert(a);
}

void PHAnimatorPool::removeAnimator(PHImageAnimator * a)
{
    animators.erase(a);
}

PHAnimatorPool * PHAnimatorPool::mainAnimatorPool()
{
    static PHAnimatorPool * main = NULL;
    if (!main)
        main = new PHAnimatorPool;
    return main;
}


void PHAnimatorPool::advanceAnimation(double elapsedTime)
{
    for (set<PHImageAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        if (!((*i)->advanceManually))
            (*i)->advanceAnimation(elapsedTime);
    }
}