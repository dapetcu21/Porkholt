//
//  PHAnimator.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/22/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHANIMATOR_H
#define PHANIMATOR_H

#include "PHMain.h"

class PHAnimatorPool;
class PHAnimator : public PHObject {
protected:
    bool advanceManually;
    PHAnimatorPool * pool;
    int _tag;
    
    friend class PHAnimatorPool;
public:
    PHAnimator();
    PHAnimator(PHAnimatorPool * pool);
    virtual ~PHAnimator();
    
    virtual void advanceAnimation(ph_float elapsedTime) = 0;
    bool isAdvancingManually() { return advanceManually; }
    void setAdvanceManually(bool s) { advanceManually = s; }
    void setAnimatorPool(PHAnimatorPool * p);
    PHAnimatorPool * animatorPool() { return pool; }
    
    void setTag(int tg) { _tag = tg; }
    int tag() { return _tag; }
};

#endif
