//
//  PHLNPC.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/15/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLNPC.h"
#include "PHTrailImageView.h"
#include "PHPlayerView.h"
#include "PHMainEvents.h"
#include "PHAuxLayerView.h"
#include <Box2D/Box2D.h>
#include "PHLua.h"

#include <typeinfo>

PHLNPC::PHLNPC() : staticFace(false), trail(false), traillen(10), bodyView(NULL), faceView(NULL), worldView(NULL), fflip(false), bflip(false), aflip(false), flipped(false)
{
    _class = "PHLNPC";
}

PHLNPC::~PHLNPC()
{
    
}

void PHLNPC::updateView()
{
    PHLObject::updateView();
    //faceView->setHorizontallyFlipped(horizontallyFlipped);
    bodyView->setTrailSize(trail?traillen:0);
}

void PHLNPC::loadFromLua(lua_State * L, const string & root,b2World * world)
{
    PHLObject::loadFromLua(L,root,world);
    
    lua_getfield(L, -1, "trail");
    if (lua_isboolean(L, -1))
        trail = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "usesTrail");
    if (lua_isboolean(L, -1))
        utrail = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    if (utrail)
        trail = true;
    
    lua_getfield(L, -1, "trailLength");
    if (lua_isnumber(L, -1))
        traillen = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "staticFace");
    if (lua_isboolean(L, -1))
        staticFace = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "faceFlipping");
    if (lua_isboolean(L, -1))
        fflip = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "bodyFlipping");
    if (lua_isboolean(L, -1))
        bflip = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "automaticFlipping");
    if (lua_isboolean(L, -1))
        aflip = lua_toboolean(L, -1);
    lua_pop(L, 1);
}

void PHLNPC::loadView()
{
	view = new PHPlayerView(PHMakeRect(viewSize.x+pos.x, viewSize.y+pos.y, viewSize.width, viewSize.height));
    PHPoint center = PHMakePoint(-viewSize.x, -viewSize.y);
    view->setRotationalCenter(center);
    view->setFlipCenter(center);
	loadImages();
    bodyView = (PHTrailImageView*)(view->viewWithTag(20));
    faceView = (PHImageView*)(view->viewWithTag(21));
    if (staticFace) 
        ((PHPlayerView*)view)->setDesignatedTag(21);
    if (typeid(bodyView) == typeid(PHPlayerView))
        bodyView->setSnapshotInterval(2/(60/PHMainEvents::sharedInstance()->framesPerSecond()));
	view->setRotation(rot);
}


void PHLNPC::updatePosition()
{
    PHLObject::updatePosition();
	if (!body) return;
	if (!worldView)
    {
        bodyView->setStopView(worldView = (bodyView->superview()->superview()));
        bodyView->bindToAuxLayer(PHAuxLayerView::auxLayerViewWithName(20), worldView);
    }
	
    b2Vec2 speed = body->GetLinearVelocity();
    if (aflip && (fflip || bflip) && abs(speed.x)>=0.1)
        setFlipped(speed.x<0);
}

void PHLNPC::flip()
{
    if (!fflip && !bflip) return;
    flipped = !flipped;
    if (bflip && body)
    {
        for (int i=0; i<fixturesDefinitions.size(); i++)
        {
            b2FixtureDef * d = fixturesDefinitions[i];
            b2Shape * s = const_cast<b2Shape*>(d->shape);
            if (!s) continue;
            if (s->m_type == b2Shape::e_circle)
            {
                b2CircleShape * ss = (b2CircleShape*)s;
                ss->m_p.Set(-ss->m_p.x,ss->m_p.y);
            }
        }
        rebuildFixtures();
        view->setHorizontallyFlipped(flipped);
    } else 
    if (fflip)
    {
        faceView->setHorizontallyFlipped(flipped);
    }
}

static int PHLNPC_isFlipped(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    lua_pushboolean(L, npc->isFlipped());
    return 1;
}

static int PHLNPC_setFlipped(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    npc->setFlipped(lua_toboolean(L, 2));
    return 0;
}

static int PHLNPC_flip(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    npc->flip();
    return 0;
}

void PHLNPC::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLNPC");
    
    lua_pushcfunction(L, PHLNPC_isFlipped);
    lua_setfield(L, -2, "isFlipped");
    lua_pushcfunction(L, PHLNPC_setFlipped);
    lua_setfield(L, -2, "setFlipped");
    lua_pushcfunction(L, PHLNPC_flip);
    lua_setfield(L, -2, "flip");
    
    lua_pop(L,1);
}