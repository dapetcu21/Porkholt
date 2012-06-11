/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHShieldView.h"
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHImage.h>

void PHShieldView::attachedToGameManager()
{
    if (!imv->image())
        imv->setImage(shieldImage(gm));
}

PHShieldView::~PHShieldView()
{
    imv->release();
}

#define factor 1.5f

PHShieldView::PHShieldView(const PHRect & fr) : PHView(fr)
{
    imv = new PHImageView();
    PHRect r = _bounds;
    r.x-=r.width*(factor-1)/2;
    r.y-=r.height*(factor-1)/2;
    r.width*=factor;
    r.height*=factor;
    imv->setFrame(r);
    addChild(imv);
    
    imv->setScaleX(1.0f/1024);
    imv->setScaleY(1.0f/1024);
    
    imv->beginCinematicAnimation(1,PHCinematicAnimator::BounceFunction);
    imv->animateScale(PHSize(1024,1024));
    imv->commitCinematicAnimation();
}

PHImage * PHShieldView::shieldImage(PHGameManager * gm) 
{
    return gm->imageNamed("shield");
}

void PHShieldView::dismiss()
{
    imv->beginCinematicAnimation(0.5,PHCinematicAnimator::FadeInFunction);
    imv->animateScale(PHSize(1.0f/1024,1.0f/1024));
    imv->animationCallback(PHInvN(this,PHView::removeFromParent));
    imv->commitCinematicAnimation();
}
