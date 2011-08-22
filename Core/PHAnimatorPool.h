//
//  PHAnimatorPool.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/30/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHANIMATORPOOL_H
#define PHANIMATORPOOL_H

#include "PHMain.h"

class PHImageAnimator;
class PHAnimatorPool : public PHObject
{
private:
    set<PHImageAnimator*> animators;
    PHMutex * mutex;
public:
    PHAnimatorPool();
    ~PHAnimatorPool();
    
    void insertAnimator(PHImageAnimator * a);
    void removeAnimator(PHImageAnimator * a);
    void advanceAnimation(double elapsed);
    
    static PHAnimatorPool * mainAnimatorPool();
};

#endif
