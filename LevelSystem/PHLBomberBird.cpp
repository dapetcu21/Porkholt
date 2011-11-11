//
//  PHLBomberBird.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 11/8/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLua.h"
#include "PHLBomberBird.h"
#include "PHLCamera.h"
#include "PHLPlayer.h"
#include "PHWorld.h"
#include "PHScripting.h"
#include "PHGameManager.h"

#include <Box2D/Box2D.h>

PHLBomberBird::PHLBomberBird() : startingPoint(PHPoint(0,3)), rotationAxis(PHPoint(1,10)), attacking(false), inCamera(false), droppedCargo(false)
{
    _class = "PHLBomberBird";
}

PHLBomberBird::~PHLBomberBird()
{
    
}

void PHLBomberBird::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLMob::loadFromLua(L, world, lvlc);
    PHLuaGetPointField(startingPoint, "startingPoint");
    PHLuaGetPointField(rotationAxis, "rotationAxis");
    PHLuaGetRectField(bounds, "bounds");
}

PHLuaDefineCall(PHLBomberBird, attack);

void PHLBomberBird::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L,"PHLBomberBird");
    
    PHLuaAddMethod(PHLBomberBird, attack);
    
    lua_pop(L,1);
}

void PHLBomberBird::attack()
{
    lastCameraRect = wrld->getCamera()->bounds();
    setPosition(lastCameraRect.origin()+startingPoint);
    attacking = true;
}

void PHLBomberBird::updatePosition()
{
    PHLMob::updatePosition();
    PHPoint pos = (position()-lastCameraRect.origin()-rotationAxis).rotated(M_PI*0.3/_gameManager->framesPerSecond())+rotationAxis;
    lastCameraRect = wrld->getCamera()->bounds();
    pos+=lastCameraRect.origin();
    if (PHRectIntersectsRect(bounds+pos,lastCameraRect))
    {
        inCamera = true;
    } else {
        if (inCamera)
        {
            destroy();
            return;
        }
    }
    if (!droppedCargo && (pos.x>wrld->getPlayer()->position().x))
    {
        droppedCargo = true;
        lua_State * L = wrld->getScripting()->luaState();
        lua_getglobal(L, "objectWithClass");
        lua_pushstring(L, "PHLEggBomb");
        PHLuaCall(L, 1, 1);
        if (lua_istable(L,-1))
        {
            PHPoint(wrld->getPlayer()->position().x,pos.y).saveToLua(L);
            lua_setfield(L, -2, "pos");
            lua_getfield(L, -1, "class");
            string clss = "PHLEggBomb";
            if (lua_isstring(L, -1))
                clss = lua_tostring(L, -1);
            lua_pop(L,1);
            PHLObject * obj = PHLObject::objectWithClass(clss);
            obj->loadFromLua(L, wrld->getPhysicsWorld(),wrld->levelController());
            obj->defferedLoading(wrld, 3, this);
            double vel = wrld->getPlayer()->getBody()->GetLinearVelocity().x;
            if (vel<0)
                vel = 0;
            obj->getBody()->SetLinearVelocity(b2Vec2(vel*0.5,0));
            obj->release();
        }
        lua_pop(L,1);
    }
        
    setPosition(pos);
}