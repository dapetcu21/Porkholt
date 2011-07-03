//
//  PHJoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHJoint.h"
#include "PHLua.h"
#include "PHWorld.h"
#include <Box2D/Box2D.h>

#include "PHDistanceJoint.h"
#include "PHRevoluteJoint.h"
#include "PHPrismaticJoint.h"

PHJoint * PHJoint::jointWithClassAndWorld(string clss, PHWorld * world)
{
    if (clss=="PHDistanceJoint")
        return new PHDistanceJoint(world);
    if (clss=="PHRevoluteJoint")
        return new PHRevoluteJoint(world);
    if (clss=="PHPrismaticJoint")
        return new PHPrismaticJoint(world);
    return new PHJoint(world);
}

PHJoint::PHJoint(PHWorld * wrld) : world(wrld), joint(NULL), body1(NULL), body2(NULL), worldCoord(false), collideConnected(true)
{
    world->addJoint(this);
}

PHJoint::~PHJoint()
{
    destroyJoint();
    setObject1(NULL);
    setObject2(NULL);
}

void PHJoint::loadFromLua(lua_State * L)
{
    lua_pushstring(L, "body1");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
    {
        lua_pushstring(L, "index");
        lua_gettable(L, -2);
        if (lua_isnumber(L, -1))
        {
            int index = lua_tonumber(L, -1);
            setObject1(world->objects[index]);
        }
        lua_pop(L,1);
    }
    lua_pop(L,1);
    
    lua_pushstring(L, "body2");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
    {
        lua_pushstring(L, "index");
        lua_gettable(L, -2);
        if (lua_isnumber(L, -1))
        {
            int index = lua_tonumber(L, -1);
            setObject2(world->objects[index]);
        }
        lua_pop(L,1);
    }
    lua_pop(L,1);
    
    worldCoord = false;
    lua_pushstring(L, "worldCoordinates");
    lua_gettable(L, -2);
    if (lua_isboolean(L, -1))
        worldCoord = lua_toboolean(L, -1);
    lua_pop(L,1);
    
    collideConnected = true;
    lua_pushstring(L, "collideConnected");
    lua_gettable(L, -2);
    if (lua_isboolean(L, -1))
        collideConnected = lua_toboolean(L, -1);
    lua_pop(L,1);
}

void PHJoint::recreateJoint()
{
    destroyJoint();
    //placeholder
}

void PHJoint::destroyJoint()
{
    if (joint)
    {
        world->physicsWorld->DestroyJoint(joint);
        joint = NULL;
    }
}

void PHJoint::setObject1(PHLObject*obj)
{
    destroyJoint();
    if (body1)
        body1->removeJoint(this);
    body1 = obj;
    if (body1)
        body1->addJoint(this);
}

void PHJoint::setObject2(PHLObject*obj)
{
    destroyJoint();
    if (body2)
        body2->removeJoint(this);
    body2 = obj;
    if (body2)
        body2->addJoint(this);
}