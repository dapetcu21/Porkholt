/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGPlayer.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGPlayer", IGPlayer)

IGPlayer::IGPlayer(IGWorld * w) : IGProp(w)
{
}

IGPlayer::~IGPlayer()
{
}

void IGPlayer::attachedToWorld()
{
    b2BodyDef def;
    def.fixedRotation = true;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(0.4, 0.4);
//    shape.m_p.Set(0, 0);
//    shape.m_radius = 0.5;

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 1.0f;
    body->CreateFixture(&fdef);
}

void IGPlayer::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-0.4 ,-0.4, 0.8, 0.8));
    iv->setImage(world->gameManager()->imageNamed("player"));
    d->addChild(iv);
    iv->release();
}

//--- Lua Scripting ---

void IGPlayer::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGPlayer");

    lua_pop(L, 1);
}
