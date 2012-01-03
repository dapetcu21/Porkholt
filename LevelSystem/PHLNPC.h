//
//  PHLNPC.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/15/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLNPC_H
#define PHLNPC_H

#include "PHLObject.h"
#define PH_GOD_MODE

class PHTrailImageView;
class PHDialog;
class PHTextView;
class PHDialogView;

class PHLNPC : public PHLObject
{
protected:
    PHImageView * bodyView;
    PHView * faceView;
    
private:
    bool trail,utrail;
    int traillen;
    bool staticFace;
    PHView * worldView;
    bool fflip,aflip;
    bool trailPossible;
    bool canBlink;
    
    
    PHPoint overHeadPoint;
    bool hover;
    bool _idle;
    
    double hoverAmmount;
public:
    PHLNPC();
    virtual ~PHLNPC();
    
    void setIdle(bool i);
    void setHovers(bool hv) { hover = hv; }
    bool hovers() { return hover; }
    void setTrail(bool tr) { trail = tr; }
    bool hasTrail() { return trail; }
    void setUsesTrail(bool tr) { utrail = tr; }
    bool usesTrail() { return utrail; }
    void setFaceFlipping(bool tr) { fflip = tr; }
    bool faceFlipping() { return fflip; }
    void setAutomaticFlipping(bool tr) { aflip = tr; }
    bool automaticFlipping() { return aflip; }
    void setTrailLength(int len) { traillen = len; }
    int trailLength() { return traillen; }
    
    virtual void updatePosition();
    virtual void updateView();
    virtual void loadView();
    virtual void loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc);
    static void registerLuaInterface(lua_State * L);
    
private:
    PHDialog * currentDialog;
    PHDialogView * dialogView;
    PHTextView * dialogTextView;
    void _dialogDismissed(PHLObject * sender, void * ud);
    void dialogViewFired(PHDialogView * dv);
    void _dialogSwapBegin(PHLObject * sender, void * ud);
    void _dialogSwapEnd(PHLObject * sender, void * ud);
    friend class PHDialogView;
    void _dialogViewFired(PHObject * sender, PHDialogView * dv);
    
    PHDialogView * questView;
    
    bool quest1,quest2,quest3,reallyquest,animatingquest;
    void showQuest();
    void hideQuest();
    void questShowedUp(PHObject * sender, void * ud);
    void questHiddenItself(PHObject * sender, void * ud);
    void updateShowsQuest()
    {
        if ((quest1 && quest2 && quest3) != reallyquest)
        {
            reallyquest = quest1 && quest2 && quest3;
            if (reallyquest)
                showQuest();
            else
                hideQuest();
        }
    }
    void setInternalShowsQuest(bool q) { quest2 = q; updateShowsQuest(); }
    bool internalShowsQuest() { return quest2; }
    void setInternalShowsQuest2(bool q) { quest3 = q; updateShowsQuest(); }
    bool internalShowsQuest2() { return quest3; }
    friend class PHWorld;
    
    bool qquest,queuedquest,showDialogDelayed;
    double questHeight;
    PHPoint questPoint;
    
    PHLAnimation * brakeAnimation;
    
    void showDialog(PHDialog * dialog);
    void swapDialog(PHDialog * dialog);
    void dismissDialog();
    
public:
    void setShowsQuest(bool q) { quest1 = q; updateShowsQuest(); }
    bool showsQuest() { return quest1; }
    bool reallyShowsQuest() { return reallyquest; }
    void queueSetShowsQuest(bool q) { qquest = q; queuedquest = true; }
    
    void setDialog(PHDialog * dialog);
    
    void walk(const PHPoint & offset, double speed) { walkTo(pos+offset,speed,NULL); }
    void walkTo(const PHPoint &  destination, double speed) { walkTo(destination,speed,NULL); }
    void walk(const PHPoint &  offset, double speed, lua_State * l) { walkTo(pos+offset,speed,l); }
    void walkTo(const PHPoint &  destination, double speed, lua_State * l);
    
    bool braked() { return brakeAnimation!=NULL; }
    void setBraked(bool br); 
    virtual void flip();
    
protected:
    double hp;
    double maxHP;
    bool invuln,hinvuln;
    double hInvulnTime;
    double hInvulnRemTime;
    PHColor hInvulnFadeColor;
    
    virtual void lowHP();
    virtual void increasedHP();
    virtual void decreasedHP();
    
#ifdef PH_GOD_MODE
    void lowHPGod();
#endif
    
    void animateHurtInvuln();
    void _animateHurtInvuln(PHObject * sender, void * ud);
    
public:
    void setHP(double HP) { hp = HP; if (hp<=0) { hp=0; lowHP(); } if (hp>maxHP) hp=maxHP; }
#ifdef PH_GOD_MODE 
#define lowHP lowHPGod
#endif
    void decreaseHP(double HP) { if (isInvulnerable()) return; hp-=HP; if (hp<=0) { hp = 0; lowHP(); } else decreasedHP(); }
#undef lowHP
    void increaseHP(double HP) { hp+=HP; if (hp>maxHP) hp=maxHP; increasedHP(); }
    double healthPoints() { return hp; }
    double maximumHP() { return maxHP; }
    void setMaximumHP(double mhp) { maxHP = mhp; if (hp>maxHP) hp=maxHP; }
    
    virtual void die();
    
    void setInvulnerable(bool i) { invuln = i; }
    bool invulnerable() { return invuln; }
    virtual bool isInvulnerable() { return invuln || hinvuln; }
    bool hurt() { return hinvuln; }
};

#endif