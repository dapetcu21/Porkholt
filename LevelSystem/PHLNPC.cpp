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
#include "PHDialog.h"
#include "PHLAnimation.h"

#include <typeinfo>

PHLNPC::PHLNPC() : staticFace(false), trail(false), traillen(10), bodyView(NULL), faceView(NULL), worldView(NULL), fflip(false), bflip(false), aflip(false), flipped(false), currentDialog(NULL), dialogView(NULL), dialogTextView(NULL), overHeadPoint(0,1), quest1(false), quest2(true), reallyquest(false), questView(NULL), animatingquest(false), qquest(false), queuedquest(false), questHeight(0.25), questPoint(0,1), shouldFlipUponLoad(false), brakeAnimation(NULL), quest3(true), showDialogDelayed(false)
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
    if (questView)
    {
        questView->removeFromSuperview();
        questView->release();
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
    
    shouldFlipUponLoad = false;
    lua_getfield(L, -1, "flipped");
    if (lua_isboolean(L, -1))
        shouldFlipUponLoad = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "automaticFlipping");
    if (lua_isboolean(L, -1))
        aflip = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "overHead");
    if (lua_istable(L, -1))
        overHeadPoint = PHPoint::pointFromLua(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "questPoint");
    if (lua_istable(L, -1))
        questPoint = PHPoint::pointFromLua(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, -1, "questHeight");
    if (lua_isnumber(L, -1))
        questHeight = lua_tonumber(L, -1);
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
    {
        bool flipped = dialogView->horizontallyFlipped();
        dialogView->setPosition(PHMakePoint(pos.x+(flipped?-1:1)*overHeadPoint.x-(flipped?dialogView->frame().width:0), pos.y+overHeadPoint.y));
    }
    if (questView)
    {
        double width = questView->frame().width;
        questView->setFrame(PHMakeRect(pos.x+(flipped?(-questPoint.x-width):(questPoint.x)), pos.y+questPoint.y, width, questHeight));
        questView->setHorizontallyFlipped(flipped);
        questView->setScalingCenter(PHOriginPoint);
    }
    if (queuedquest)
    {
        queuedquest = false;
        setShowsQuest(qquest);
    }
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
	if (shouldFlipUponLoad)
    {
        setFlipped(shouldFlipUponLoad);
        shouldFlipUponLoad = false;
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

void PHLNPC::setDialog(PHDialog * dialog)
{
    if (!dialog)
    {
        if (currentDialog)
            dismissDialog();
        return;
    }
    if (currentDialog)
        swapDialog(dialog);
    else
        showDialog(dialog);
}

#define dialogFontSize 0.2
#define dialogBorderTop 0.2
#define dialogBorderBottom 0.2
#define dialogBorderLeft 0.15
#define dialogBorderright 0.15

void PHLNPC::showDialog(PHDialog *dialog)
{
    dialog->retain();
    if (currentDialog)
    {
        currentDialog->release();
    }
    currentDialog = dialog;
    setInternalShowsQuest2(false);
    if (animatingquest)
    {
        showDialogDelayed = true;
        return;
    }
    if (!dialogView)
    {
        dialogView = new PHDialogView(this);
        dialogView->setImage(PHLevelController::dialogImage);
        dialogView->setEffectOrder(PHView::EffectOrderRotateFlipScale);
        dialogView->setUserInput(true);
        dialogView->setStretchBubble(true);
        dialogTextView = new PHTextView();
        dialogTextView->setFont(PHFont::fontNamed("Arial"));
        dialogTextView->setEffectOrder(PHView::EffectOrderRotateFlipScale);
        dialogTextView->setFontSize(dialogFontSize);
        dialogTextView->setFontColor(PHBlackColor);
        dialogTextView->setAlignment(PHTextView::alignBottom | PHTextView::justifyLeft);
        dialogView->addSubview(dialogTextView);
        dialogTextView->release();
        getWorld()->getWorldView()->addSubview(dialogView);
    }
    PHView::cancelAllAnimationsWithTag(5836);
    PHPoint bubblePoint = pos;
    bubblePoint.y+=overHeadPoint.y;
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
    dialogView->setFrame(PHMakeRect(bubblePoint.x-(flipped?width:0)+(flipped?-1:1)*overHeadPoint.x, bubblePoint.y, width, height));
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
    anim->tag = 5836;
    anim->timeFunction = PHAnimationDescriptor::BounceFunction;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::_dialogDismissed(PHLObject * sender, void * ud)
{
    dialogView->removeFromSuperview();
    dialogView->release();
    dialogView=NULL;
    setInternalShowsQuest2(true);
}

void PHLNPC::dialogViewFired(PHDialogView * dv)
{
    if (dialogView==dv)
    {
        if (currentDialog->inStack)
            getWorld()->advanceDialog();
        else
            dismissDialog();
    }
    if (questView==dv)
    {
        PHLuaGetWeakRef(L, this);
        lua_getfield(L, -1, "questTapped");
        lua_pushvalue(L, -2);
        PHLuaCall(L, 1, 0);
        dv->reenableTouch();
    }
}

void PHLNPC::dismissDialog()
{
    if (currentDialog)
    {
        currentDialog->callback();
        currentDialog->release();
        currentDialog = NULL;
    }
    if (!dialogView) return;
    dialogView->cancelAnimationsWithTag(5836);
    dialogTextView->cancelAnimationsWithTag(5836);
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
    anim->tag = 5838;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::swapDialog(PHDialog *dialog)
{
    if (currentDialog)
    {
        currentDialog->callback();
        currentDialog->release();
    }
    currentDialog = dialog;
    if (currentDialog)
        currentDialog->retain();
    else
    {
        dismissDialog();
        return;
    }
    dialogView->cancelAnimationsWithTag(5836);
    dialogTextView->cancelAnimationsWithTag(5836);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->customColor = PHClearColor;
    anim->time = 0.2f;
    anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    anim->target = this;
    anim->callback = (PHCallback)&PHLNPC::_dialogSwapBegin;
    anim->userdata = NULL;
    anim->view = dialogTextView;
    anim->tag = 5838;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::_dialogSwapBegin(PHLObject * sender, void * ud)
{
    PHPoint bubblePoint = pos;
    bubblePoint.y+=overHeadPoint.y;
    bool flipped = dialogView->horizontallyFlipped();
    PHLCamera * camera = getWorld()->getCamera();
    double camwidth = camera->width();
    double twidth = (flipped?1:-1)*(position().x-camera->position().x)+camwidth*0.45;
    if (twidth>camwidth*0.6)
        twidth=camwidth*0.6;
    PHRect f = dialogTextView->frame();
    f.x = twidth*dialogBorderLeft;
    f.width = twidth*(1.0f-dialogBorderLeft-dialogBorderright);
    dialogTextView->setFrame(f);
    dialogTextView->setText(currentDialog->text);
    PHPoint sz =  dialogTextView->textSize();
    double owidth = dialogView->frame().width;
    double oheight = dialogView->frame().height;
    double width = sz.x/(1.0f-dialogBorderLeft-dialogBorderright);
    double height = sz.y/(1.0f-dialogBorderTop-dialogBorderBottom);
    dialogView->setFrame(PHMakeRect(bubblePoint.x-(flipped?width:0)+(flipped?-1:1)*overHeadPoint.x, bubblePoint.y, width, height));
    dialogTextView->setFrame(PHMakeRect(width*dialogBorderLeft, height*dialogBorderBottom, sz.x, sz.y));
    
    dialogView->setScalingCenter(PHOriginPoint);
    dialogView->setScaleX(owidth/width);
    dialogView->setScaleY(oheight/height);
    
    if ((owidth>=width && oheight>=height) || (owidth/width >=1 && owidth/width>=height/oheight) || (oheight/height >=1 && oheight/height>=width/owidth))
    {
        dialogTextView->setScaleX(width/owidth);
        dialogTextView->setScaleY(height/oheight);
        PHAnimationDescriptor * anim = new PHAnimationDescriptor;
        anim->scaleX = owidth/width;
        anim->scaleY = oheight/height;
        anim->time = 0.5;
        anim->tag = 5838;
        anim->timeFunction = PHAnimationDescriptor::FadeInOutFunction;
        anim->view = dialogTextView;
        PHView::addAnimation(anim);
        anim->release();
    }
    
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->scaleX = width/owidth;
    anim->scaleY = height/oheight;
    anim->time = 0.5;
    anim->tag = 5838;
    anim->timeFunction = PHAnimationDescriptor::FadeInOutFunction;
    anim->view = dialogView;
    anim->target = this;
    anim->callback = (PHCallback)&PHLNPC::_dialogSwapEnd;
    anim->userdata = NULL;
    PHView::addAnimation(anim);
    anim->release();
    
    anim = new PHAnimationDescriptor;
    anim->customColor = PHBlackColor;
    anim->time = 0.2f;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    anim->view = dialogTextView;
    anim->tag = 5838;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::_dialogSwapEnd(PHLObject * sender, void * ud)
{
    if (dialogView)
        dialogView->reenableTouch();
}

void PHLNPC::showQuest()
{
    if (!questView)
    {
        questView = new PHDialogView(this);
        questView->setImage(PHLevelController::questImage);
        questView->setEffectOrder(PHView::EffectOrderRotateFlipScale);
        getWorld()->getWorldView()->addSubview(questView);
    }
    questView->cancelAnimationsWithTag(4867);
    questView->setUserInput(true);
    
    double aspectRatio = PHLevelController::questImage?((double)(PHLevelController::questImage->width())/PHLevelController::questImage->height()):1.0f;
    
    questView->setFrame(PHMakeRect(pos.x+(flipped?(-questPoint.x-questHeight*aspectRatio):(questPoint.x)), pos.y+questPoint.y, questHeight*aspectRatio, questHeight));
    questView->setHorizontallyFlipped(flipped);
    questView->setScalingCenter(PHOriginPoint);

    double scale = 1024;
    questView->setScaleX(1/scale);
    questView->setScaleY(1/scale);
    animatingquest = true;
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->scaleX = scale;
    anim->scaleY = scale;
    anim->tag = 4867;
    anim->view = questView;
    anim->callback = (PHCallback)&PHLNPC::questShowedUp;
    anim->target = this;
    anim->userdata = NULL;
    anim->time = 0.5;
    anim->timeFunction = PHAnimationDescriptor::BounceFunction;
    PHView::addAnimation(anim);
    anim->release();
}

void PHLNPC::questShowedUp(PHObject *sender, void *ud)
{
    animatingquest = false;
    if (showDialogDelayed)
    {
        showDialogDelayed = false;
        showDialog(currentDialog);
    }
}

void PHLNPC::hideQuest()
{
    if (!questView)
    {
        animatingquest = false;
        if (showDialogDelayed)
        {
            showDialogDelayed = false;
            showDialog(currentDialog);
        }
        return;
    }
    questView->cancelAnimationsWithTag(4867);
    questView->setUserInput(false);
    
    double scale = 1024;
    animatingquest = true;
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->scaleX = 1/scale;
    anim->scaleY = 1/scale;
    anim->tag = 4867;
    anim->view = questView;
    anim->callback = (PHCallback)&PHLNPC::questHiddenItself;
    anim->target = this;
    anim->userdata = NULL;
    anim->time = 0.3;
    anim->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    PHView::addAnimation(anim);
    anim->release();

}

void PHLNPC::questHiddenItself(PHObject *sender, void *ud)
{
    animatingquest = false;
    if (showDialogDelayed)
    {
        showDialogDelayed = false;
        showDialog(currentDialog);
    }
    if (questView)
    {
        questView->removeFromSuperview();
        questView->release();
        questView = NULL;
    }
}

class PHLWalkAnimation : public PHLAnimation
{
private:
    PHLNPC * npc;
    PHPoint destination;
    bool leftSide;
    lua_State * L;
    virtual void animationStepped(double elapsed)
    {
        if (brakeForce())
        {
            if (npc->scalarVelocity()==0)
            {
                invalidate();
                if (L)
                {
                    lua_getglobal(L,"PHCallbackHelper");
                    PHLuaGetHardRef(L,this);
                    PHLuaCall(L,1,0);
                    PHLuaDeleteHardRef(L,this);
                    L = NULL;
                }
            }
        } else
        if (leftSide?(npc->position().x>=destination.x):(npc->position().x<=destination.x))
        {
            invalidate();
            PHLWalkAnimation * anim = new PHLWalkAnimation(npc);
            anim->setBrakeForce(npc->mass()*6);
            anim->setTime(INFINITY);
            anim->setCurveFunction(PHLAnimation::ConstantFunction);
            anim->setDisableDynamics(false);
            PHLuaGetHardRef(L,this);
            anim->setLuaCallback(L);
            npc->addAnimation(anim);
            anim->release();
        }
    }
    void setLuaCallback(lua_State * l)
    {
        L = l;
        if (!L) return;
        PHLuaSetHardRef(L,this);
    }
    friend class PHLNPC;
public:
    PHLWalkAnimation(PHLNPC * n) : npc(n), leftSide(true), L(NULL) {};\
    ~PHLWalkAnimation() { if (L) PHLuaDeleteHardRef(L,this); }
};

void PHLNPC::walkTo(const PHPoint &  destination, double speed, lua_State * l)
{
    if (!body) return;
    PHPoint velocity = destination-pos;
    velocity.normalize();
    velocity*=speed;
    PHLWalkAnimation * anim = new PHLWalkAnimation(this);
    anim->setVelocity(velocity,mass()*20);
    anim->setTime(INFINITY);
    anim->setCurveFunction(PHLAnimation::ConstantFunction);
    anim->setDisableDynamics(false);
    anim->destination = destination;
    anim->leftSide = pos.x<destination.x;
    anim->setLuaCallback(l);
    addAnimation(anim);
    anim->release();
}

void PHLNPC::setBraked(bool b)
{
    if (b)
    {
        if (brakeAnimation) return;
        brakeAnimation = new PHLAnimation;
        brakeAnimation->setBrakeForce(mass()*6);
        brakeAnimation->setTime(INFINITY);
        brakeAnimation->setCurveFunction(PHLAnimation::ConstantFunction);
        brakeAnimation->setDisableDynamics(false);
        addAnimation(brakeAnimation);
    } else {
        if (!brakeAnimation) return;
        brakeAnimation->invalidate();
        brakeAnimation->release();
        brakeAnimation = NULL;
    }
}

#pragma mark -
#pragma mark scripting

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
    if (lua_istable(L, 3))
    {
        lua_pushvalue(L, 3);
        dialog->setCallback(L);
    }
    npc->getWorld()->addDialog(dialog);
    dialog->release();
    return 0;
}

static int PHLNPC_setDialog(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checkstring(L, 2);
    PHDialog * dialog = new PHDialog;
    dialog->text = lua_tostring(L, 2);
    dialog->npc = npc;
    if (lua_istable(L, 3))
    {
        lua_pushvalue(L, 3);
        dialog->setCallback(L);
    }
    npc->setDialog(dialog);
    dialog->release();
    return 0;
}

static int PHLNPC_showsQuest(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    lua_pushboolean(L, npc->showsQuest());
    return 1;
}

static int PHLNPC_reallyShowsQuest(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    lua_pushboolean(L, npc->reallyShowsQuest());
    return 1;
}

static int PHLNPC_setShowsQuest(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    npc->queueSetShowsQuest(lua_toboolean(L, 2));
    return 0;
}

static int PHLNPC_walk(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    double speed = 2;
    if (lua_isnumber(L,3))
        speed = lua_tonumber(L,3);
    if (lua_istable(L,4))
    {
        lua_pushvalue(L,4);
        npc->walk(PHPoint::pointFromLua(L,2),speed,L);
    } else
        npc->walk(PHPoint::pointFromLua(L,2),speed);
    return 0;
}

static int PHLNPC_walkTo(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    double speed = 2;
    if (lua_isnumber(L,3))
        speed = lua_tonumber(L,3);
    if (lua_istable(L,4))
    {
        lua_pushvalue(L,4);
        npc->walk(PHPoint::pointFromLua(L,2),speed,L);
    } else
        npc->walk(PHPoint::pointFromLua(L,2),speed);
    return 0;
}

static int PHLNPC_braked(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    lua_pushboolean(L, npc->braked());
    return 1;
}

static int PHLNPC_setBraked(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    npc->setBraked(lua_toboolean(L, 2));
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
    lua_setfield(L, -2, "_addDialog");
    lua_pushcfunction(L, PHLNPC_setDialog);
    lua_setfield(L, -2, "_setDialog");
    lua_pushcfunction(L, PHLNPC_showsQuest);
    lua_setfield(L, -2, "showsQuest");
    lua_pushcfunction(L, PHLNPC_setShowsQuest);
    lua_setfield(L, -2, "setShowsQuest");
    lua_pushcfunction(L, PHLNPC_reallyShowsQuest);
    lua_setfield(L, -2, "reallyShowsQuest");
    lua_pushcfunction(L, PHLNPC_walk);
    lua_setfield(L, -2, "_walk");
    lua_pushcfunction(L, PHLNPC_walkTo);
    lua_setfield(L, -2, "_walkTo");
    lua_pushcfunction(L, PHLNPC_braked);
    lua_setfield(L, -2, "braked");
    lua_pushcfunction(L, PHLNPC_setBraked);
    lua_setfield(L, -2, "setBraked");
    
    
    lua_pop(L,1);
}