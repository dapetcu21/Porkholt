/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Box2D/Box2D.h>
#include "IGScreenBounds.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHView.h>

IGSCRIPTING_REGISTERCLASS("IGScreenBounds", IGScreenBounds)

IGScreenBounds::IGScreenBounds(IGWorld * w) : IGProp(w)
{
}

IGScreenBounds::~IGScreenBounds()
{
}

void IGScreenBounds::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGScreenBounds");

    lua_pop(L, 1);
}

void IGScreenBounds::attachedToWorld()
{
    PHRect f = world->view()->bounds();
    b2BodyDef def;
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    b2Vec2 v[4];
    v[0].Set(f.width+0.5, 0);
    v[1].Set(f.width+0.6, 0);
    v[2].Set(f.width+0.6, f.height);
    v[3].Set(f.width+0.5, f.height);
    shape.Set(v, 4);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.isSensor = true;
    fdef.filter.categoryBits = IGWorld::collisionEnv;
    fdef.filter.maskBits = IGWorld::collisionAllEnv;
    body->CreateFixture(&fdef);

    v[0].Set(-0.6, 0);
    v[1].Set(-0.5, 0);
    v[2].Set(-0.5, f.height);
    v[3].Set(-0.6, f.height);
    shape.Set(v, 4);
    body->CreateFixture(&fdef);
}
