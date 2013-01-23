/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGKnifeMob.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGKnifeMob", IGKnifeMob)

IGKnifeMob::IGKnifeMob(IGWorld * w) : IGMob(w)
{
}

IGKnifeMob::~IGKnifeMob()
{
}

#define my (0.25 * 0.8)
#define mx (my * (1483.0 / 1080))

#define dx 0.363778 
#define dy -0.104883
#define dif b2Vec2(-dx * mx, -dy * my)
#define phdif PHPoint(-dx * mx, -dy * my)


void IGKnifeMob::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);

    b2CircleShape circle;
    circle.m_p = b2Vec2(0, 0);
    circle.m_radius = 0.5*mx;

    b2FixtureDef fdef;
    fdef.shape = &circle;
    fdef.density = 10.0f;
    fdef.filter.categoryBits = IGWorld::collisionMobs;
    fdef.filter.maskBits = IGWorld::collisionPlayer | IGWorld::collisionPlayerBullets | IGWorld::collisionEnv;
    body->CreateFixture(&fdef);
    
    b2PolygonShape shape;
    b2Vec2 v2[] = {
        b2Vec2(mx*-0.334588, my*0.935352) + dif,
        b2Vec2(mx*-0.837514, my*0.693594) + dif,
        b2Vec2(mx*-0.985412, my*0.058594) + dif,
        b2Vec2(mx*-0.797557, my*-0.805957) + dif,
        b2Vec2(mx*-0.391491, my*-0.976250) + dif,
        b2Vec2(mx*-0.141136, my*-0.001621) + dif,
    };
    shape.Set(v2, 6);
    fdef.shape = &shape;
    fdef.density = 3.0f;
    body->CreateFixture(&fdef);

    b2CircleShape bounding;
    bounding.m_radius = mx*1.5;
    fdef.density = 1.0f;
    fdef.filter.categoryBits = IGWorld::collisionMobs;
    fdef.filter.maskBits = IGWorld::collisionEnv;
    fdef.filter.groupIndex = 12;
    fdef.friction = 0;
    fdef.shape = &bounding;
    body->CreateFixture(&fdef);

    createDampingJoint();
}

void IGKnifeMob::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-mx, -my, 2*mx, 2*my) + phdif);
    iv->setImage(world->gameManager()->imageNamed("knife"));
    d->addChild(iv);
    PHView * v = new PHView(PHRect(-0.1,-0.1, 0.2, 0.2));
    v->setBackgroundColor(PHColor(1,0,0,1));
    //d->addChild(v);
    v->release();
    iv->release();
}

//--- Lua Scripting ---

void IGKnifeMob::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGKnifeMob");

    lua_pop(L, 1);
}
