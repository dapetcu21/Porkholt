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

void IGBasicMob::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(0.4, 0.4);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 1.0f;
    body->CreateFixture(&fdef);

    createDampingJoint();
}

void IGBasicMob::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-0.4 ,-0.4, 0.8, 0.8));
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
