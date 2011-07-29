//
//  PHShieldView.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHShieldView.h"
#include "PHImageView.h"
#include "PHImage.h"
#include "PHAnimationDescriptor.h"

PHImage * PHShieldView::img = NULL;

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
    imv->setImage(shieldImage());
    addSubview(imv);
    
    imv->setScaleX(1.0f/1024);
    imv->setScaleY(1.0f/1024);
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->time = 1;
    anim->timeFunction = PHAnimationDescriptor::BounceFunction;
    anim->scaleX = 1024;
    anim->scaleY = 1024;
    anim->view = imv;
    PHView::addAnimation(anim);
    anim->release();
    
}

PHShieldView::~PHShieldView()
{
    imv->release();
}

PHImage * PHShieldView::shieldImage() {
    if (!img)
    {
        img = PHImage::imageNamed("shield");
        img->retain();
    }
    return img;
}

void PHShieldView::shieldImageRelease()
{
    img = (PHImage*)img->release();
}

void PHShieldView::dismiss()
{
    PHAnimationDescriptor * anim = new PHAnimationDescriptor;
    anim->time = 0.5;
    anim->timeFunction = PHAnimationDescriptor::FadeInFunction;
    anim->scaleX = 1.0f/1024;
    anim->scaleY = 1.0f/1024;
    anim->view = imv;
    anim->callback = (PHCallback)&PHView::removeFromSuperview;
    anim->target = this;
    PHView::addAnimation(anim);
    anim->release();
}