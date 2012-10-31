/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGBasicMob.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGBasicMob", IGBasicMob)

IGBasicMob::IGBasicMob(IGWorld * w) : IGMob(w)
{
}

IGBasicMob::~IGBasicMob()
{
}

#define mx (0.15 * (894.0 / 441))
#define my (0.15)

void IGBasicMob::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(mx, my);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 1.0f;
    fdef.filter.categoryBits = IGWorld::collisionMobs;
    fdef.filter.maskBits = IGWorld::collisionAllMob;
    body->CreateFixture(&fdef);

    createDampingJoint();
}

void IGBasicMob::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-mx, -my, 2*mx, 2*my));
    iv->setImage(world->gameManager()->imageNamed("mob"));
    d->addChild(iv);
    iv->release();
}

void IGBasicMob::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBasicMob");

    lua_pop(L, 1);
}
