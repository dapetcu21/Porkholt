//
//  PHAnimator.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/22/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#include "PHAnimator.h"
#include "PHAnimatorPool.h"

#define INIT advanceManually(false), pool(NULL), _tag(0)

PHAnimator::PHAnimator() : INIT
{
    setAnimatorPool(PHAnimatorPool::currentAnimatorPool());
}

PHAnimator::PHAnimator(PHAnimatorPool * pool) : INIT
{
    setAnimatorPool(pool);
}

PHAnimator::~PHAnimator()
{
    if (pool)
        pool->removeAnimator(this);
}

void PHAnimator::setAnimatorPool(PHAnimatorPool * p)
{
    if (pool)
        pool->removeAnimator(this);
    pool = p;
    if (pool)
        pool->insertAnimator(this);
}