/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGBackgroundParticles.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHParticleView.h>
#include <Porkholt/Core/PHGenericParticleAnimator.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGBackgroundParticles", IGBackgroundParticles)

IGBackgroundParticles::IGBackgroundParticles(IGWorld * w) : IGObject(w)
{
}

IGBackgroundParticles::~IGBackgroundParticles()
{
}

PHDrawable * IGBackgroundParticles::loadDrawable()
{
    PHRect b = world->view()->bounds();
    PHParticleView * pv = new PHParticleView(b);
    pv->setImage(world->gameManager()->imageNamed("cell"));
    PHGenericParticleAnimator * pa = new PHGenericParticleAnimator();
    pa->setGenerationArea(PHRect(b.width + 0.5, 0, 0, b.height));
    pa->setInitColor(PHColor(1, 1, 1, 0.3));
    pa->setFinalColor(PHColor(1, 1, 1, 0.3));
    pa->setInitSize(PHSize(0.5, 0.5));
    pa->setFinalSize(PHSize(0.5, 0.5));
    pa->setVelocity(PHPoint(-0.7, 0));
    pa->setVelocityVariation(0.2);
    pa->setParticlesPerSecond(2);
    pa->setSpreadAngle(M_PI/10);
    pa->setRotatesWithVelocity(true);
    pa->setGravity(PHPoint(0, 0));
    pa->setParticleLifetime(15);
    pa->advanceAnimation(15);

    pv->setParticleAnimator(pa);
    pa->release();
    return pv;
}

//--- Lua Scripting ---

void IGBackgroundParticles::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBackgroundParticles");

    lua_pop(L, 1);
}
