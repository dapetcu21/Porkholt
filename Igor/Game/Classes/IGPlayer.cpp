/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGPlayer.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGPlayer", IGPlayer)

#define dy 0.32
//#define dx (dy * (288.0/327))
#define dx dy
//#define dx (dy * (423.0/466))

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
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);

    b2PolygonShape shape;
    shape.SetAsBox(dx, dy);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.density = 1.0f;
    fdef.filter.categoryBits = IGWorld::collisionPlayer;
    fdef.filter.maskBits = IGWorld::collisionAllPlayer;
    body->CreateFixture(&fdef);
}

void IGPlayer::configureDrawable(PHDrawable * d)
{
    PHImageView * iv = new PHImageView(PHRect(-dx ,-dy, 2*dx, 2*dy));
    iv->setImage(world->gameManager()->imageNamed("player"));
    iv->setMaterial(world->gameManager()->materialNamed("density"));
    d->addChild(iv);
    iv->release();
}

void IGPlayer::loseHealth(ph_float h)
{
    hp -= h;
    if (hp <= 0)
        die();
}

void IGPlayer::die()
{
    if (L)
    {
        PHLuaGetWeakRef(L, this);
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "dieCallback");
            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, -2);
                PHLuaCall(L, 1, 0);
                lua_pop(L, 1);
                return;
            }
            lua_pop(L, 1);
        } 
        lua_pop(L, 1);
    }
    removeFromWorld();
}



//--- Lua Scripting ---

PHLuaNumberGetter(IGPlayer, health);
PHLuaNumberSetter(IGPlayer, setHealth);
PHLuaNumberSetter(IGPlayer, loseHealth);
PHLuaDefineCall(IGPlayer, die);

void IGPlayer::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGPlayer");

    PHLuaAddMethod(IGPlayer, health);
    PHLuaAddMethod(IGPlayer, setHealth);
    PHLuaAddMethod(IGPlayer, loseHealth);
    PHLuaAddMethod(IGPlayer, die);

    lua_pop(L, 1);
}
