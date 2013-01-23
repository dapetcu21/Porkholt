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

#define my (0.15)
#define mx (my * (1000.0/700))

#define dx -0.6
#define dy 0.5
#define dif b2Vec2(-dx * mx, -dy * my)
#define phdif PHPoint(-dx * mx, -dy * my)

void IGBasicMob::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    def.fixedRotation = true;
    body = world->physicsWorld()->CreateBody(&def);

    b2CircleShape circle;
    circle.m_p = b2Vec2(mx*-0.720120, my*0.440059) + dif;
    circle.m_radius = 0.3*mx;

    b2FixtureDef fdef;
    fdef.shape = &circle;
    fdef.density = 10.0f;
    fdef.filter.categoryBits = IGWorld::collisionMobs;
    fdef.filter.maskBits = IGWorld::collisionAllMob;
    body->CreateFixture(&fdef);
    
    b2PolygonShape shape;
    b2Vec2 v2[] = {
        b2Vec2(mx*-0.478212, my*0.344492) + dif,
        b2Vec2(mx*0.527344, my*-0.917617) + dif,
        b2Vec2(mx*0.962404, my*-0.246855) + dif,
        b2Vec2(mx*0.787953, my*0.271484) + dif,
    };
    shape.Set(v2, 4);
    fdef.shape = &shape;
    fdef.density = 3.0f;
    body->CreateFixture(&fdef);

    createDampingJoint();
}

void IGBasicMob::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-mx, -my, 2*mx, 2*my) + phdif);
    iv->setImage(world->gameManager()->imageNamed("basic_mob"));
    d->addChild(iv);
    PHView * v = new PHView(PHRect(-0.1,-0.1, 0.2, 0.2));
    v->setBackgroundColor(PHColor(1,0,0,1));
    //d->addChild(v);
    v->release();
    iv->release();
}

void IGBasicMob::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGBasicMob");

    lua_pop(L, 1);
}
