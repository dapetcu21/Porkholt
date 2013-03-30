/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */


#ifndef PHANIMATORPOOL_H
#define PHANIMATORPOOL_H

#include <Porkholt/Core/PHMain.h>

class PHAnimator;
class PHAnimatorPool : public PHObject
{
private:
    set<PHAnimator*> animators,insertQueue,deleteQueue;
    bool insideJob;
    PHMutex * mutex;
    
public:
    PHAnimatorPool();
    ~PHAnimatorPool();
    
    void insertAnimator(PHAnimator * a);
    void addAnimator(PHAnimator * a) { insertAnimator(a); }
    void removeAnimator(PHAnimator * a);
    void removeAllAnimators();
    void removeAnimatorsWithTag(int d);
    void advanceAnimation(float elapsed);
    
    
    //waitForIt means that scheduleAction should block until the
    //invocation is performed and should only be used from 
    //another thread than the one on which the animator pool 
    //is running to avoid a deadlock
    void scheduleAction(const PHInvocation & inv, double time, bool repeat, bool waitForIt);
    void scheduleAction(const PHInvocation & inv, double time, bool repeat) {
        scheduleAction(inv, time, repeat, false);
    }
    void scheduleAction(const PHInvocation & inv, double time) {
        scheduleAction(inv, time, false, false);
    }
    void scheduleAction(const PHInvocation & inv) {
        scheduleAction(inv, 0, false, false);
    }
    void scheduleAction(const PHInvocation & inv, bool waitForIt) {
        scheduleAction(inv, 0, false, waitForIt);
    }

};

#endif
