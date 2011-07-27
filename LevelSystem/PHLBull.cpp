//
//  PHLBull.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/24/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLBull.h"
#include "PHLua.h"
#include "PHLAnimation.h"
#include "PHTimer.h"
#include "PHWorld.h"
#include "PHLPlayer.h"
#include "PHImageView.h"
#include "PHImageAnimator.h"

PHLBull::PHLBull() : PHLMob(), attackRange(3.5f), attackVelocity(5.0f), attackDuration(0.5f), unrageTime(1.0f), attacking(false), cooldownDuration(2.0f)
{
    _class = "PHLBull";
}

PHLBull::~PHLBull()
{
    
}

void PHLBull::cooldownEnded(PHObject * sender, void * ud)
{
    attacking = false;
}

void PHLBull::attacked(PHObject * sender, void * ud)
{
    PHImageAnimator * animator;
    if (faceView && ((animator=faceView->animator())))
        animator->animateSection("relaxing");
}

void PHLBull::reallyAttack(PHObject * sender, void * ud)
{
    PHPoint v = PHPoint(attackVelocity*(isFlipped()?-1:1), 0);
    
    PHLAnimation * anim = new PHLAnimation;
    anim->setVelocity(v,mass()*20);
    anim->setTime(attackDuration);
    anim->setCurveFunction(PHLAnimation::ConstantFunction);
    anim->setDisableDynamics(false);
    addAnimation(anim);
    anim->release();
    
    PHTimer * timer = new PHTimer;
    timer->setTimeInterval(attackDuration+cooldownDuration);
    timer->setCallback(this, (PHCallback)&PHLBull::cooldownEnded, NULL);
    getWorld()->scheduleTimer(timer);
    timer->release();
    
    timer = new PHTimer;
    timer->setTimeInterval(unrageTime);
    timer->setCallback(this, (PHCallback)&PHLBull::attacked, NULL);
    getWorld()->scheduleTimer(timer);
    timer->release();
}

void PHLBull::attack()
{
    setFlipped((getWorld()->getPlayer()->position()-position()).x<0);
    
    PHImageAnimator * animator;
    if (faceView&& ((animator=faceView->animator())))
        animator->animateSection("charging",this,(PHCallback)&PHLBull::reallyAttack,NULL);
    else
        reallyAttack(NULL,NULL);
    attacking = true;
}

void PHLBull::updatePosition()
{
    PHLMob::updatePosition();
    if (!attacking && fabs((getWorld()->getPlayer()->position()-position()).x)<=attackRange)
        attack();
}

bool PHLBull::vulnerableFixture(b2Fixture * f)
{
    if (f==fixtures[0]) return false;
    return true;
}

double PHLBull::speedNeededForDamagingFixture(b2Fixture * f)
{
    return 2.0f;
}

void PHLBull::loadFromLua(lua_State *L, const string &root, b2World *world)
{
    PHLMob::loadFromLua(L, root, world);
    
    PHLuaGetNumberField(attackRange, "attackRange");
    PHLuaGetNumberField(attackVelocity, "attackVelocity");
    PHLuaGetNumberField(attackDuration, "attackDuration");
    PHLuaGetNumberField(cooldownDuration, "cooldownDuration");
    PHLuaGetNumberField(unrageTime, "relaxTime");
}

void PHLBull::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L,"PHLBull");
    
    lua_pop(L,1);
}