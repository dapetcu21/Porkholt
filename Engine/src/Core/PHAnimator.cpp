/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>

#define INIT advanceManually(false), retained(false), pool(NULL), _tag(0), poolc(0)

PHAnimator::PHAnimator() : INIT
{
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

void PHAnimator::setRetainedInThePool(bool r)
{
    if (retained == r) return;
    retained = r;
    if (r)
        for (int i = 0; i<poolc; i++)
            retain();
    else
        for (int i = 0; i<poolc; i++)
            release();
}

void PHAnimator::retainInPool()
{
    poolc++;
    if (retained)
        retain();
}

void PHAnimator::releaseInPool()
{
    poolc--;
    if (retained)
        release();
}
