/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGDeadpool.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGDeadpool", IGDeadpool)

#define my (0.32 * 1.5)
#define mx (my*(423.0/466))

#define cx 0.8
#define cy cx

#define dx -0.8
#define dy 0.8

IGDeadpool::IGDeadpool(IGWorld * w) : IGMob(w)
{
}

IGDeadpool::~IGDeadpool()
{
}

void IGDeadpool::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    def.fixedRotation = true;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(mx, my);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 10.0f;
    fdef.filter.categoryBits = IGWorld::collisionMobs;
    fdef.filter.maskBits = IGWorld::collisionAllMob;
    body->CreateFixture(&fdef);
    
    createDampingJoint();
}

void IGDeadpool::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-mx, -my, 2*mx, 2*my));
    iv->setImage(world->gameManager()->imageNamed("deadpool"));
    iv->setHorizontallyFlipped(true);
    d->addChild(iv);
    iv->release();
}

void IGDeadpool::dialog1()
{
    PHImageView * iv = new PHImageView(PHRect(-cx, -cy, 2*cx, 2*cy) + PHPoint(dx, dy));
    dialog = iv;
    iv->setImage(world->gameManager()->imageNamed("deadpool_comic1"));
    iv->setScale(PHSize(0.01, 0.01));
    iv->setScalingCenter(PHPoint(1.8*cx, cy*0.2));
    iv->beginCinematicAnimation(0.5, PHCinematicAnimator::BounceFunction);
    iv->animateScale(PHSize(100, 100));
    iv->commitCinematicAnimation();
    getDrawable()->addChild(iv);
    iv->release();
}

void IGDeadpool::dialog2()
{
    dialog->beginCinematicAnimation(0.5, PHCinematicAnimator::FadeInFunction);
    dialog->animateScale(PHSize(0.01, 0.01));
    dialog->animationCallback(PHInvBind(dialog, PHDrawable::removeFromParent, NULL));
    dialog->commitCinematicAnimation();

    PHImageView * iv = new PHImageView(PHRect(-cx, -cy, 2*cx, 2*cy) + PHPoint(dx, dy));
    iv->setScalingCenter(PHPoint(1.8*cx, cy*0.2));
    iv->setImage(world->gameManager()->imageNamed("deadpool_comic2"));
    iv->setScale(PHSize(0.01, 0.01));
    iv->beginCinematicAnimation(0.5);
    iv->chainCinematicAnimation(0.5, PHCinematicAnimator::BounceFunction);
    iv->animateScale(PHSize(100, 100));
    iv->commitCinematicAnimation();
    getDrawable()->addChild(iv);
    iv->release();
}

//--- Lua Scripting ---

PHLuaDefineCall(IGDeadpool, dialog1);
PHLuaDefineCall(IGDeadpool, dialog2);

void IGDeadpool::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGDeadpool");

    PHLuaAddMethod(IGDeadpool, dialog1);
    PHLuaAddMethod(IGDeadpool, dialog2);

    lua_pop(L, 1);
}
