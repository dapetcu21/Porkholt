/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHANIMATOR_H
#define PHANIMATOR_H

#include <Porkholt/Core/PHMain.h>

class PHAnimatorPool;
class PHAnimator : public PHObject {
protected:
    bool advanceManually, retained;
    PHAnimatorPool * pool;
    size_t _tag;
    
    friend class PHAnimatorPool;
    
    int poolc;
    void retainInPool();
    void releaseInPool();

public:
    PHAnimator();
    PHAnimator(PHAnimatorPool * pool);
    virtual ~PHAnimator();
    
    virtual void advanceAnimation(float elapsedTime) = 0;
    bool isAdvancingManually() { return advanceManually; }
    void setAdvanceManually(bool s) { advanceManually = s; }
    void setAnimatorPool(PHAnimatorPool * p);
    PHAnimatorPool * animatorPool() { return pool; }
    
    bool retainedInThePool() { return retained; }
    void setRetainedInThePool(bool r);
    
    void setTag(size_t tg) { _tag = tg; }
    size_t tag() { return _tag; }
};

#endif
