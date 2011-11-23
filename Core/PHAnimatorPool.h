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

class PHAnimator;
class PHAnimatorPool : public PHObject
{
private:
    set<PHAnimator*> animators,insertQueue,deleteQueue;
    bool insideJob;
    PHMutex * mutex;
    
    static map < PHThread*, list<PHAnimatorPool*> > stacks;
    static PHMutex * staticMutex;
public:
    PHAnimatorPool();
    ~PHAnimatorPool();
    
    void insertAnimator(PHAnimator * a);
    void removeAnimator(PHAnimator * a);
    void advanceAnimation(double elapsed);
    
    void push();
    void pop() { popPool(); }
    static void popPool();
    
    static PHAnimatorPool * mainAnimatorPool();
    static PHAnimatorPool * currentAnimatorPool();
};

#endif
