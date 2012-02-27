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
#include "PHGameManager.h"
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
#include "PHGameManager.h"
#include "PHEventQueue.h"
#include "PHAnimatedImage.h"
#include "PHImage.h"
#include "PHImageAnimator.h"
#include "PHImageView.h"
#include "PHLPlayer.h"
#include "PHAnimatorPool.h"

#include <typeinfo>

PHL_REGISTEROBJECT(PHLNPC)

PHLNPC::PHLNPC() : staticFace(false), trail(false), traillen(10), bodyView(NULL), faceView(NULL), worldView(NULL), fflip(false), utrail(false), aflip(false), currentDialog(NULL), dialogView(NULL), dialogTextView(NULL), overHeadPoint(0,1), quest1(false), quest2(true), reallyquest(false), questView(NULL), animatingquest(false), qquest(false), queuedquest(false), questHeight(0.25), questPoint(0,1), brakeAnimation(NULL), quest3(true), showDialogDelayed(false), hp(1.0f),maxHP(1.0f), invuln(false), hinvuln(false), hInvulnTime(1.0f), hInvulnRemTime(0), hInvulnFadeColor(PHBlackColor), hover(false), hoverAmmount(0), _idle(false), canBlink(false)
{
    _class = "PHLNPC";
}

PHLNPC::~PHLNPC()
{
    if (faceView)
    {
        faceView->removeFromSuperview();
        faceView->release();
    }
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

void PHLNPC::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLObject::loadFromLua(L,world,lvlc);
    
    PHLuaGetBoolField(trail,"trail");
    PHLuaGetBoolField(utrail,"usesTrail");
    if (utrail)       
        trail = true;
    PHLuaGetNumberField(traillen, "trailLength");
    PHLuaGetBoolField(staticFace,"staticFace");
    PHLuaGetBoolField(fflip,"faceFlipping");
    PHLuaGetBoolField(aflip,"automaticFlipping");
    PHLuaGetPointField(overHeadPoint,"overHead");
    PHLuaGetPointField(questPoint,"questPoint");
    PHLuaGetNumberField(questHeight, "questHeight");
    PHLuaGetNumberField(hInvulnTime, "hurtInvulnerableTime");
    PHLuaGetNumberField(maxHP, "healthPoints");
    PHLuaGetBoolField(hover, "hovers");
    PHLuaGetBoolField(canBlink, "canBlink");
    hp = maxHP;
    PHLuaGetNumberField(questHeight, "questHeight");
    lua_getfield(L, -1, "startingHealthPoints");
    if (lua_isnumber(L, -1))
        setHP(lua_tonumber(L, -1));
    lua_pop(L, 1);
}

void PHLNPC::loadView()
{
	view = new PHPlayerView(PHRect(viewSize.x+pos.x, viewSize.y+pos.y, viewSize.width, viewSize.height));
    PHPoint center = PHPoint(-viewSize.x, -viewSize.y);
    view->setRotationalCenter(center);
    view->setFlipCenter(center);
	loadImages();
    bodyView = (PHImageView*)(view->viewWithTag(20));
    list<PHView*> * l = view->viewsWithTag(21);
    for(list<PHView*>::iterator i = l->begin(); i!=l->end(); i++)
    {
        PHView * v=*i;
        if (!v) break;
        if (!faceView)
        {
            faceView = new PHView(view->bounds());
            faceView->setRotationalCenter(center);
            faceView->setFlipCenter(center);
            faceView->setTag(-21);
            view->addSubview(faceView);
        }
        v->retain();
        v->removeFromSuperview();
        faceView->addSubview(v);
        v->release();
    };
    delete l;
    if (staticFace) 
        ((PHPlayerView*)view)->setDesignatedTag(-21);
    if ((trailPossible = (dynamic_cast<PHTrailImageView*>(bodyView)!=NULL)))
        ((PHTrailImageView*)bodyView)->setSnapshotInterval(2/(60/gm->framesPerSecond()));
    view->setGameManager(gm);
	view->setRotation(rot);
}


void PHLNPC::updateView()
{
    if (hover)
    {
        offset.y = sin(hoverAmmount)*0.1;
        ph_float frameInterval = 1.0f/gm->framesPerSecond();
        hoverAmmount = PHWarp(hoverAmmount+frameInterval*2, M_PI*2);
    }
    PHLObject::updateView();
    if (dialogView)
    {
        bool flipped = dialogView->horizontallyFlipped();
        dialogView->setPosition(PHPoint(pos.x+(flipped?-1:1)*overHeadPoint.x-(flipped?dialogView->frame().width:0), pos.y+overHeadPoint.y));
    }
    if (questView)
    {
        ph_float width = questView->frame().width;
        questView->setFrame(PHRect(pos.x+(flipped?(-questPoint.x-width):(questPoint.x)), pos.y+questPoint.y, width, questHeight));
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
    b2Vec2 speed = body->GetLinearVelocity();
    ph_float elapsed = 1.0f/gm->framesPerSecond();
    if (aflip && abs(speed.x)>=0.1)
        setFlipped(speed.x<0);
    setIdle(abs(speed.x)<0.1);
    if (hInvulnRemTime)
    {
        hInvulnRemTime-=elapsed;
        if (hInvulnRemTime<=0)
        {
            hInvulnRemTime = 0;
            hinvuln = false;
        }
    }
}

void PHLNPC::setIdle(bool i)
{
    if (i==_idle) return;
    _idle = i;
    if (!canBlink) return;
    if (!faceView) return;
    const list<PHView*> & l = faceView->subViews();
    list<PHView*>::const_iterator j;
    for (j = l.begin(); j!=l.end(); j++)
    {
        PHImageView * v= dynamic_cast<PHImageView*>(*j);
        if (v)
        {
            PHImageAnimator * a = v->animator();
            if (a)
            {
                a->animateSection(_idle?"idle":"moving");
            }
        }
    }
}

void PHLNPC::flip()
{
    if (fflip)
    {
        flipped = !flipped;
        if (faceView)
            faceView->setHorizontallyFlipped(flipped);
    } else
        PHLObject::flip();
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

#define dialogFontSize 0.15
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
        dialogView->mutex();
        dialogView->setImage(gm->imageNamed("dialogbubble"));
        dialogView->setEffectOrder(PHView::EffectOrderRotateFlipScale);
        dialogView->setUserInput(true);
        dialogView->setStretchBubble(true);
        dialogTextView = new PHTextView();
        dialogTextView->setFont(gm->fontNamed("BDCartoonShout"));
        dialogTextView->setEffectOrder(PHView::EffectOrderRotateFlipScale);
        dialogTextView->setFontSize(dialogFontSize);
        dialogTextView->setFontColor(PHBlackColor);
        dialogTextView->setAlignment(PHTextView::alignBottom | PHTextView::justifyLeft);
        dialogView->addSubview(dialogTextView);
        dialogTextView->release();
        getWorld()->getWorldView()->addSubview(dialogView);
    }
    dialogView->mutex()->lock();
    PHAnimatorPool::currentAnimatorPool()->removeAnimatorsWithTag(5836);
    PHPoint bubblePoint = pos;
    bubblePoint.y+=overHeadPoint.y;
    PHLCamera * camera = getWorld()->getCamera();
    ph_float camwidth = camera->width();
    bool flipped = position().x>=camera->position().x;
    ph_float width = abs(position().x-camera->position().x)+camwidth*0.45;
    if (width>camwidth*0.6)
        width=camwidth*0.6;
    ph_float height = dialogFontSize/(1.0f-dialogBorderTop-dialogBorderBottom);
    dialogView->setFrame(PHRect(bubblePoint.x, bubblePoint.y, width, height));
    dialogView->setFlipCenter(PHOriginPoint);
    dialogView->setHorizontallyFlipped(flipped);
    dialogTextView->setHorizontallyFlipped(flipped);
    dialogTextView->setFrame(PHRect(width*dialogBorderLeft, height*dialogBorderBottom, width*(1.0f-dialogBorderLeft-dialogBorderright), dialogFontSize));
    dialogTextView->setText(currentDialog->text);
    PHPoint sz =  dialogTextView->textSize();
    width = sz.x/(1.0f-dialogBorderLeft-dialogBorderright);
    height = sz.y/(1.0f-dialogBorderTop-dialogBorderBottom);
    dialogView->setFrame(PHRect(bubblePoint.x-(flipped?width:0)+(flipped?-1:1)*overHeadPoint.x, bubblePoint.y, width, height));
    dialogTextView->setFrame(PHRect(width*dialogBorderLeft, height*dialogBorderBottom, sz.x, sz.y));
    ph_float scale = 1024;
    dialogView->setScaleX(1/scale);
    dialogView->setScaleY(1/scale);
    dialogView->setScalingCenter(PHOriginPoint);
    dialogView->beginCinematicAnimation(0.5f,PHCinematicAnimator::BounceFunction);
    dialogView->animateScale(PHSize(scale,scale));
    dialogView->animationTag(5836);
    dialogView->commitCinematicAnimation();
    dialogView->mutex()->unlock();
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
    getWorld()->modelEventQueue()->schedule(PHInv(this, PHLNPC::_dialogViewFired, dv), false);
}

void PHLNPC::_dialogViewFired(PHObject * sender, PHDialogView * dv)
{
    if (dialogView==dv)
    {
        PHDialog * d = currentDialog;
        d->retain();
        currentDialog->callback();
        if (d->inStack)
            getWorld()->advanceDialog();
        else
            if (d==currentDialog)
                dismissDialog();
        d->release();
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
    dialogView->mutex()->lock();
    dialogView->removeCinematicAnimationsWithTag(5836);
    dialogTextView->removeCinematicAnimationsWithTag(5836);
    dialogView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeInFunction);
    ph_float scale = 1024;
    dialogView->animateScale(PHSize(1/scale,1/scale));
    dialogView->animationCallback(PHInvN(this, PHLNPC::_dialogDismissed));
    dialogView->animationTag(5838);
    dialogView->commitCinematicAnimation();
    dialogView->mutex()->unlock();
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
    dialogView->mutex()->lock();
    dialogView->removeCinematicAnimationsWithTag(5836);
    dialogTextView->removeCinematicAnimationsWithTag(5836);
    dialogTextView->beginCinematicAnimation(0.2f,PHCinematicAnimator::FadeOutFunction);
    dialogTextView->animateCustomColor(PHClearColor);
    dialogTextView->animationCallback(PHInvN(this, PHLNPC::_dialogSwapBegin));
    dialogTextView->animationTag(5838);
    dialogTextView->commitCinematicAnimation();
    dialogView->mutex()->unlock();
}

void PHLNPC::_dialogSwapBegin(PHLObject * sender, void * ud)
{
    dialogView->mutex()->lock();
    PHPoint bubblePoint = pos;
    bubblePoint.y+=overHeadPoint.y;
    bool flipped = dialogView->horizontallyFlipped();
    PHLCamera * camera = getWorld()->getCamera();
    ph_float camwidth = camera->width();
    ph_float twidth = (flipped?1:-1)*(position().x-camera->position().x)+camwidth*0.45;
    if (twidth>camwidth*0.6)
        twidth=camwidth*0.6;
    PHRect f = dialogTextView->frame();
    f.x = twidth*dialogBorderLeft;
    f.width = twidth*(1.0f-dialogBorderLeft-dialogBorderright);
    dialogTextView->setFrame(f);
    dialogTextView->setText(currentDialog->text);
    PHPoint sz =  dialogTextView->textSize();
    ph_float owidth = dialogView->frame().width;
    ph_float oheight = dialogView->frame().height;
    ph_float width = sz.x/(1.0f-dialogBorderLeft-dialogBorderright);
    ph_float height = sz.y/(1.0f-dialogBorderTop-dialogBorderBottom);
    dialogView->setFrame(PHRect(bubblePoint.x-(flipped?width:0)+(flipped?-1:1)*overHeadPoint.x, bubblePoint.y, width, height));
    dialogTextView->setFrame(PHRect(width*dialogBorderLeft, height*dialogBorderBottom, sz.x, sz.y));
    
    dialogView->setScalingCenter(PHOriginPoint);
    dialogView->setScaleX(owidth/width);
    dialogView->setScaleY(oheight/height);
    
    if ((owidth>=width && oheight>=height) || (owidth/width >=1 && owidth/width>=height/oheight) || (oheight/height >=1 && oheight/height>=width/owidth))
    {
        dialogTextView->setScaleX(width/owidth);
        dialogTextView->setScaleY(height/oheight);
        dialogTextView->beginCinematicAnimation(0.5f,PHCinematicAnimator::FadeInOutFunction);
        dialogTextView->animateScale(PHSize(owidth/width,oheight/height));
        dialogTextView->animationTag(5838);
        dialogTextView->commitCinematicAnimation();
    }
    
    dialogView->beginCinematicAnimation(0.5f, PHCinematicAnimator::FadeInOutFunction);
    dialogView->animateScale(PHSize(width/owidth,height/oheight));
    dialogView->animationTag(5838);
    dialogView->animationCallback(PHInvN(this, PHLNPC::_dialogSwapEnd));
    dialogView->commitCinematicAnimation();
    
    dialogTextView->beginCinematicAnimation(0.2f, PHCinematicAnimator::FadeInFunction);
    dialogTextView->animateCustomColor(PHBlackColor);
    dialogTextView->animationTag(5838);
    dialogTextView->commitCinematicAnimation();
    dialogView->mutex()->unlock();
}

void PHLNPC::_dialogSwapEnd(PHLObject * sender, void * ud)
{
    if (dialogView)
        dialogView->reenableTouch();
}

void PHLNPC::showQuest()
{
    PHImage * qi = gm->imageNamed("quest");
    if (!questView)
    {
        questView = new PHDialogView(this);
        questView->mutex();
        questView->setImage(qi);
        questView->setEffectOrder(PHView::EffectOrderRotateFlipScale);
        getWorld()->getWorldView()->addSubview(questView);
    }
    questView->mutex()->lock();
    questView->removeCinematicAnimationsWithTag(4867);
    questView->setUserInput(true);
    
    ph_float aspectRatio = qi?((ph_float)(qi->width())/qi->height()):1.0f;
    
    questView->setFrame(PHRect(pos.x+(flipped?(-questPoint.x-questHeight*aspectRatio):(questPoint.x)), pos.y+questPoint.y, questHeight*aspectRatio, questHeight));
    questView->setHorizontallyFlipped(flipped);
    questView->setScalingCenter(PHOriginPoint);

    ph_float scale = 1024;
    questView->setScaleX(1/scale);
    questView->setScaleY(1/scale);
    animatingquest = true;
    questView->beginCinematicAnimation(0.5f,PHCinematicAnimator::BounceFunction);
    questView->animateScale(PHSize(scale,scale));
    questView->animationTag(4867);
    questView->animationCallback(PHInvN(this,PHLNPC::questShowedUp));
    questView->commitCinematicAnimation();
    questView->mutex()->unlock();
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
    questView->mutex()->lock();
    questView->removeCinematicAnimationsWithTag(4867);
    questView->setUserInput(false);
    
    ph_float scale = 1024;
    animatingquest = true;
    questView->beginCinematicAnimation(0.3f,PHCinematicAnimator::FadeOutFunction);
    questView->animateScale(PHSize(1/scale,1/scale));
    questView->animationTag(4867);
    questView->animationCallback(PHInvN(this, PHLNPC::questHiddenItself));
    questView->commitCinematicAnimation();
    questView->mutex()->unlock();

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
    virtual void animationStepped(ph_float elapsed)
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
            anim->setBrakeForce(npc->mass()*9);
            anim->setTime(INFINITY);
            anim->setCurveFunction(PHLAnimation::ConstantFunction);
            anim->setDisableDynamics(false);
            if (L)
            {
                PHLuaGetHardRef(L,this);
                anim->setLuaCallback(L);
                PHLuaDeleteHardRef(L,this);
                L = NULL;
            }
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

void PHLNPC::walkTo(const PHPoint &  destination, ph_float speed, lua_State * l)
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
        brakeAnimation->setBrakeForce(mass()*9);
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

void PHLNPC::die()
{
    poof();
}

void PHLNPC::lowHP()
{
    die();
}

#ifdef PH_GOD_MODE
void PHLNPC::lowHPGod()
{
    if (dynamic_cast<PHLPlayer*>(this))
        PHLog("You should have died, bastard!");
    else
        lowHP();
}
#endif

void PHLNPC::increasedHP()
{
    
}

void PHLNPC::decreasedHP()
{
    animateHurtInvuln();
}


void PHLNPC::_animateHurtInvuln(PHObject * sender, void * ud)
{
    if (hInvulnFadeColor == PHInvalidColor) return;
    if (!bodyView) return;
    bodyView->setTintColor(PHWhiteColor);
    bodyView->beginCinematicAnimation(hInvulnTime/2,PHCinematicAnimator::FadeOutFunction);
    bodyView->animateCustomColor(hInvulnFadeColor);
    bodyView->chainCinematicAnimation(hInvulnTime/2,PHCinematicAnimator::FadeInFunction);
    bodyView->animateCustomColor(PHWhiteColor);
    bodyView->commitCinematicAnimation();
}

void PHLNPC::animateHurtInvuln()
{
    hInvulnRemTime = hInvulnTime;
    hinvuln = true;
    if (hInvulnFadeColor == PHInvalidColor) return;
    if (!bodyView) return;
    
    getWorld()->viewEventQueue()->schedule(PHInv(this,PHLNPC::_animateHurtInvuln,NULL),false);
}

#pragma mark -
#pragma mark scripting

PHLuaBoolGetter(PHLNPC, hasTrail);
PHLuaBoolSetter(PHLNPC, setTrail);
PHLuaBoolGetter(PHLNPC, usesTrail);
PHLuaBoolSetter(PHLNPC, setUsesTrail);

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

PHLuaBoolGetter(PHLNPC, showsQuest);
PHLuaBoolGetter(PHLNPC, reallyShowsQuest);

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
    ph_float speed = 2;
    if (lua_isnumber(L,3))
        speed = lua_tonumber(L,3);
    if (lua_istable(L,4))
    {
        lua_pushvalue(L,4);
        npc->walk(PHPoint::fromLua(L,2),speed,L);
    } else
        npc->walk(PHPoint::fromLua(L,2),speed);
    return 0;
}

static int PHLNPC_walkTo(lua_State * L)
{
    PHLNPC * npc = (PHLNPC*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    ph_float speed = 2;
    if (lua_isnumber(L,3))
        speed = lua_tonumber(L,3);
    if (lua_istable(L,4))
    {
        lua_pushvalue(L,4);
        npc->walkTo(PHPoint::fromLua(L,2),speed,L);
    } else
        npc->walkTo(PHPoint::fromLua(L,2),speed);
    return 0;
}

PHLuaBoolGetter(PHLNPC, braked);
PHLuaBoolSetter(PHLNPC, setBraked);
PHLuaNumberSetter(PHLNPC, setHP);
PHLuaNumberSetter(PHLNPC, setMaximumHP);
PHLuaNumberSetter(PHLNPC, increaseHP);
PHLuaNumberSetter(PHLNPC, decreaseHP);
PHLuaNumberGetter(PHLNPC, healthPoints);
PHLuaNumberGetter(PHLNPC, maximumHP);
PHLuaDefineCall(PHLNPC, die);
PHLuaBoolGetter(PHLNPC, invulnerable);
PHLuaBoolGetter(PHLNPC, isInvulnerable);
PHLuaBoolSetter(PHLNPC, setInvulnerable);
PHLuaBoolGetter(PHLNPC, hovers);
PHLuaBoolSetter(PHLNPC, setHovers);

void PHLNPC::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHLNPC");
    
    PHLuaAddMethod(PHLNPC, hasTrail);
    PHLuaAddMethod(PHLNPC, setTrail);
    PHLuaAddMethod(PHLNPC, usesTrail);
    PHLuaAddMethod(PHLNPC, setUsesTrail);
    PHLuaAddMethod_(PHLNPC, addDialog);
    PHLuaAddMethod_(PHLNPC, setDialog);
    PHLuaAddMethod(PHLNPC, showsQuest);
    PHLuaAddMethod(PHLNPC, setShowsQuest);
    PHLuaAddMethod(PHLNPC, reallyShowsQuest);
    PHLuaAddMethod_(PHLNPC, walkTo);
    PHLuaAddMethod_(PHLNPC, walk);
    PHLuaAddMethod(PHLNPC, braked);
    PHLuaAddMethod(PHLNPC, setBraked);
    PHLuaAddMethod(PHLNPC, hovers);
    PHLuaAddMethod(PHLNPC, setHovers);
    
    PHLuaAddMethod(PHLNPC, healthPoints);
    PHLuaAddMethod(PHLNPC, setHP);
    PHLuaAddMethod(PHLNPC, increaseHP);
    PHLuaAddMethod(PHLNPC, decreaseHP);
    PHLuaAddMethod(PHLNPC, maximumHP);
    PHLuaAddMethod(PHLNPC, setMaximumHP);
    PHLuaAddMethod(PHLNPC, die);
    PHLuaAddMethod(PHLNPC, invulnerable);
    PHLuaAddMethod(PHLNPC, isInvulnerable);
    PHLuaAddMethod(PHLNPC, setInvulnerable);
    
    lua_pop(L,1);
}