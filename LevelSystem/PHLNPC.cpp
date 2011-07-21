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
#include "PHTextView.h"
#include "PHLevelController.h"
#include "PHWorld.h"
#include "PHLCamera.h"
#include "PHDialogView.h"

#include <typeinfo>

PHLNPC::PHLNPC() : staticFace(false), trail(false), traillen(10), bodyView(NULL), faceView(NULL), worldView(NULL), fflip(false), bflip(false), aflip(false), flipped(false), currentDialog(NULL), dialogView(NULL), dialogTextView(NULL), overHeadPoint(0,1)
{
    _class = "PHLNPC";
}

PHLNPC::~PHLNPC()
{
    if (dialogView)
    {
        dialogView->removeFromSuperview();
        dialogView->release();
    }
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
    
    lua_getfield(L, -1, "overHead");
    if (lua_istable(L, -1))
        overHeadPoint = PHPoint::pointFromLua(L, -1);
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
    if ((trailPossible = (dynamic_cast<PHTrailImageView*>(bodyView)!=NULL)))
        ((PHTrailImageView*)bodyView)->setSnapshotInterval(2/(60/PHMainEvents::sharedInstance()->framesPerSecond()));
	view->setRotation(rot);
}


void PHLNPC::updateView()
{
    PHLObject::updateView();
    if (dialogView)
        dialogView->setPosition(overHeadPoint+pos);
    if (trailPossible)
        ((PHTrailImageView*)bodyView)->setTrailSize((utrail&&trail)?traillen:0);
}

void PHLNPC::updatePosition()
{
    PHLObject::updatePosition();
	if (!body) return;
	if (!worldView && trailPossible)
    {
        ((PHTrailImageView*)bodyView)->setStopView(worldView = (bodyView->superview()->superview()));
        ((PHTrailImageView*)bodyView)->bindToAuxLayer(PHAuxLayerView::auxLayerViewWithName(20), worldView);
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
            if (s->m_type == b2Shape::e_polygon)
            {
                b2PolygonShape * ss = (b2PolygonShape*)s;
                int n = ss->GetVertexCount();
                b2Vec2 * v = new b2Vec2[n];
                for (int i=0; i<n; i++)
                {
                    v[i]=ss->GetVertex(i);
                    v[i].x = -v[i].x;
                }
                ss->Set(v,n);
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

#define dialogFontSize 0.2
#define dialogBorderTop 0.2
#define dialogBorderBottom 0.2
#define dialogBorderLeft 0.2
#define dialogBorderright 0.2

void PHLNPC::showDialog(PHDialog *dialog)
{
    if (currentDialog)
        currentDialog->release();
    currentDialog = dialog;
    currentDialog->retain();
    if (!dialogView)
    {
        dialogView = new PHDialogView(this);
        dialogView->setImage(PHLevelController::dialogImage);
        dialogView->setUserInput(true);
        dialogTextView = new PHTextView();
        dialogTextView->setFont(PHFont::fontNamed("Arial"));
        dialogTextView->setFontSize(dialogFontSize);
        dialogTextView->setFontColor(PHBlackColor);
        dialogTextView->setAlignment(PHTextView::alignBottom | PHTextView::justifyLeft);
        dialogView->addSubview(dialogTextView);
        dialogTextView->release();
        getWorld()->getWorldView()->addSubview(dialogView);
    }
    dialogView->cancelAnimations();
    PHPoint bubblePoint = overHeadPoint+pos;
    PHLCamera * camera = getWorld()->getCamera();
    double camwidth = camera->width();
    bool flipped = position().x>=camera->position().x;
    double width = abs(position().x-camera->position().x)+camwidth*0.45;
    if (width>camwidth*0.6)
        width=camwidth*0.6;
    double height = dialogFontSize/(1.0f-dialogBorderTop-dialogBorderBottom);
    dialogView->setFrame(PHMakeRect(bubblePoint.x, bubblePoint.y, width, height));
    dialogView->setFlipCenter(PHOriginPoint);
    dialogView->setHorizontallyFlipped(flipped);
    dialogTextView->setHorizontallyFlipped(flipped);
    dialogTextView->setFrame(PHMakeRect(width*dialogBorderLeft, height*dialogBorderBottom, width*(1.0f-dialogBorderLeft-dialogBorderright), dialogFontSize));
    dialogTextView->setText(currentDialog->text);
    PHPoint sz =  dialogTextView->textSize();
    width = sz.x/(1.0f-dialogBorderLeft-dialogBorderright);
    height = sz.y/(1.0f-dialogBorderTop-dialogBorderBottom);
    dialogView->setFrame(PHMakeRect(bubblePoint.x, bubblePoint.y, width, height));
    dialogTextView->setFrame(PHMakeRect(width*dialogBorderLeft, height*dialogBorderBottom, sz.x, sz.y));
    double scale = 1024;
    dialogView->setScaleX(1/scale);
    dialogView->setScaleY(1/scale);
    dialogView->setScalingCenter(PHOriginPoint);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->scaleX = scale;
    anim->scaleY = scale;
    anim->view = dialogView;
    anim->time = 0.5f;
    anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::_dialogDismissed(PHLObject * sender, void * ud)
{
    dialogView->removeFromSuperview();
    dialogView->release();
    dialogView=NULL;
}

void PHLNPC::dialogViewFired(PHDialogView * dv)
{
    if (dialogView==dv)
        getWorld()->advanceDialog();
}

void PHLNPC::dismissDialog()
{
    if (currentDialog)
    {
        currentDialog->release();
        currentDialog = NULL;
    }
    if (!dialogView) return;
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    double scale = 1024;
    anim->scaleX = 1/scale;
    anim->scaleY = 1/scale;
    anim->time = 0.5f;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    anim->target = this;
    anim->callback = (PHCallback)&PHLNPC::_dialogDismissed;
    anim->userdata = NULL;
    anim->view = dialogView;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::swapDialog(PHDialog *dialog)
{
    
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

static int PHLNPC_hasTrail(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    lua_pushboolean(L, npc->hasTrail());
    return 1;
}

static int PHLNPC_setTrail(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    npc->setTrail(lua_toboolean(L, 2));
    return 0;
}

static int PHLNPC_usesTrail(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    lua_pushboolean(L, npc->usesTrail());
    return 1;
}

static int PHLNPC_setUsesTrail(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    npc->setUsesTrail(lua_toboolean(L, 2));
    return 0;
}

static int PHLNPC_addDialog(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checkstring(L, 2);
    PHDialog * dialog = new PHDialog;
    dialog->text = lua_tostring(L, 2);
    dialog->npc = npc;
    npc->getWorld()->addDialog(dialog);
    dialog->release();
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
    lua_pushcfunction(L, PHLNPC_hasTrail);
    lua_setfield(L, -2, "hasTrail");
    lua_pushcfunction(L, PHLNPC_setTrail);
    lua_setfield(L, -2, "setTrail");
    lua_pushcfunction(L, PHLNPC_usesTrail);
    lua_setfield(L, -2, "usesTrail");
    lua_pushcfunction(L, PHLNPC_setUsesTrail);
    lua_setfield(L, -2, "setUsesTrail");
    lua_pushcfunction(L, PHLNPC_addDialog);
    lua_setfield(L, -2, "addDialog");
    
    lua_pop(L,1);
}