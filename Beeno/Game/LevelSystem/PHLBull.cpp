/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLBull.h"
#include <Porkholt/Core/PHLua.h>
#include "PHLAnimation.h"
#include <Porkholt/Core/PHTimer.h>
#include "PHWorld.h"
#include "PHLPlayer.h"
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHImageAnimator.h>
#include <Porkholt/Core/PHAnimatorPool.h>

PHL_REGISTEROBJECT(PHLBull)

PHLBull::PHLBull() : PHLMob(), attackRange(3.5f), attackVelocity(5.0f), attackDuration(0.5f), unrageTime(1.0f), attacking(false), cooldownDuration(2.0f)
{
    _class = "PHLBull";
}

PHLBull::~PHLBull()
{
    PHImageAnimator * animator;
    PHImageView * iv;
    if (faceView && ((iv=(PHImageView*)faceView->childWithTag(21))) && ((animator=iv->animator())))
        animator->pause();
}

void PHLBull::cooldownEnded(PHObject * sender, void * ud)
{
    attacking = false;
}

void PHLBull::attacked(PHObject * sender, void * ud)
{
    PHImageAnimator * animator;
    PHImageView * iv;
    if (faceView && ((iv=(PHImageView*)faceView->childWithTag(21))) && ((animator=iv->animator())))
        animator->animateSection("relaxing");
    setBraked(true);
}

void PHLBull::reallyAttack(PHObject * sender, void * ud)
{
    PHPoint v = PHPoint(attackVelocity*(isFlipped()?-1:1), 0);
    
    setBraked(false);

    PHLAnimation * anim = new PHLAnimation;
    anim->setVelocity(v,mass()*20);
    anim->setTime(attackDuration);
    anim->setCurveFunction(PHLAnimation::ConstantFunction);
    anim->setDisableDynamics(false);
    addAnimation(anim);
    anim->release();
    
    getWorld()->modelEventQueue()->scheduleAction(PHInvBind(this, PHLBull::cooldownEnded, NULL), attackDuration+cooldownDuration);

    getWorld()->modelEventQueue()->scheduleAction(PHInvBind(this, PHLBull::attacked, NULL), unrageTime);
}

void PHLBull::attack()
{
    setFlipped((getWorld()->getPlayer()->position()-position()).x<0);
    
    PHImageAnimator * animator;
    PHImageView * iv;
    if (faceView && ((iv=(PHImageView*)faceView->childWithTag(21))) && ((animator=iv->animator())))
        animator->animateSection("charging",PHInv(this,PHLBull::reallyAttack,NULL));
    else
        reallyAttack(NULL,NULL);
    attacking = true;
}

void PHLBull::updatePosition()
{
    PHLMob::updatePosition();
    float ang = PHWarp(rotation(),M_PI*2);
    if (ang>M_PI) ang-=M_PI*2;
    if (abs(ang)>=M_PI_4)
    {
        poof();
        return;
    }
    PHPoint posi = getWorld()->getPlayer()->position();
    PHRect fr = view->frame();
    if (!attacking && posi.y>=fr.y && posi.y<=fr.y+fr.height && fabs(posi.x-position().x)<=attackRange)
        attack();
}

bool PHLBull::vulnerableFixture(b2Fixture * f)
{
    if (f==*(fixtures.begin())) return false;
    return true;
}

float PHLBull::speedNeededForDamagingFixture(b2Fixture * f)
{
    return 2.0f;
}

void PHLBull::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLMob::loadFromLua(L, world,lvlc);
    
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
