/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGWallCell.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

#define cwidth 0.5
#define cheight 0.2
#define weldpoint b2Vec2(-0.2, 0)
#define lweldpoint b2Vec2(0.2, 0)

IGSCRIPTING_REGISTERCLASS("IGWallCell", IGWallCell)

IGWallCell::IGWallCell(IGWorld * w) : IGDampingProp(w)
{
}

IGWallCell::~IGWallCell()
{
}

b2Joint * IGWallCell::weldToObject(IGObject * obj, bool last)
{
    b2WeldJointDef def;
    def.bodyA = body;
    def.bodyB = obj->physicsBody();
    def.Initialize(body, obj->physicsBody(), body->GetWorldPoint(last?lweldpoint:weldpoint));
    def.dampingRatio = 0.9;
    def.frequencyHz = 2;
    return world->physicsWorld()->CreateJoint(&def);
}

void IGWallCell::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(cwidth/2, cheight/2);
//    shape.m_p.Set(0, 0);
//    shape.m_radius = 0.5;

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 1.0f;
    fdef.filter.categoryBits = IGWorld::collisionEnv;
    fdef.filter.maskBits = IGWorld::collisionAllEnv;
    body->CreateFixture(&fdef);

    createDampingJoint();
}



void IGWallCell::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-cwidth/2, -cheight/2, cwidth, cheight));
    iv->setImage(world->gameManager()->imageNamed("pill"));
    d->addChild(iv);
    iv->release();
}

//--- Lua Scripting ---

static int IGWallCell_weldToObject(lua_State * L)
{
    IGWallCell * c = (IGWallCell*)PHLuaThisPointer(L);
    IGObject * o = (IGObject*)PHLuaThisPointer(L, 2);
    lua_pushlightuserdata(L, c->weldToObject(o, lua_toboolean(L, 3)));
    return 1;
}

static int IGWallCell_destroyWeld(lua_State * L)
{
    IGWallCell * c = (IGWallCell*)PHLuaThisPointer(L);
    b2Joint * b = (b2Joint*)lua_touserdata(L, 2);
    if (!b) return 0;
    c->getWorld()->physicsWorld()->DestroyJoint(b);
    return 0;
}

void IGWallCell::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGWallCell");

    PHLuaAddMethod(IGWallCell, weldToObject);
    PHLuaAddMethod(IGWallCell, destroyWeld);

    lua_pop(L, 1);
}
