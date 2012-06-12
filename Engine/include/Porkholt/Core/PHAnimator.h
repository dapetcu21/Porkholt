/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHANIMATOR_H
#define PHANIMATOR_H

#include <Porkholt/Core/PHMain.h>

class PHAnimatorPool;
class PHAnimator : public PHObject {
protected:
    bool advanceManually, retained;
    PHAnimatorPool * pool;
    int _tag;
    
    friend class PHAnimatorPool;
    
    int poolc;
    void retainInPool();
    void releaseInPool();

public:
    PHAnimator();
    PHAnimator(PHAnimatorPool * pool);
    virtual ~PHAnimator();
    
    virtual void advanceAnimation(ph_float elapsedTime) = 0;
    bool isAdvancingManually() { return advanceManually; }
    void setAdvanceManually(bool s) { advanceManually = s; }
    void setAnimatorPool(PHAnimatorPool * p);
    PHAnimatorPool * animatorPool() { return pool; }
    
    bool retainedInThePool() { return retained; }
    void setRetainedInThePool(bool r);
    
    void setTag(int tg) { _tag = tg; }
    int tag() { return _tag; }
};

#endif
