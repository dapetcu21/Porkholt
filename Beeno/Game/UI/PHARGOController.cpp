//
//  PHARGOController.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 12/29/11.
//  Copyright (c) 2011 Porkholt Labs!. All rights reserved.
//

#include "PHARGOController.h"
#include "PHImageView.h"
#include "PHView.h"
#include "PHParticleView.h"
#include "PHParticleAnimator.h"
#include "PHAnimatorPool.h"
#include "PHTextView.h"
#include "PHFont.h"

PHView * PHARGOController::loadView(const PHRect & frame)
{
    PHView * v = new PHView(frame);
    v->setBackgroundColor(PHBlackColor);
    PHImage * img = PHImage::imageNamed("argo");
    double scale = ((double)(img->width()))/((double)(img->height()));
    imageView = new PHImageView(PHRect(frame.width/2-100*scale,frame.height/2-100-400,200*scale,200));
    imageView->setImage(PHImage::imageNamed("argo"));
    imageView->setTintColor(PHClearColor);
    imageView->setScaleX(10);
    imageView->setScaleY(10);
    
    
    PHAnimationDescriptor * d = new PHAnimationDescriptor;
    d->moveY = 400;
    d->scaleX = 0.1;
    d->scaleY = 0.1;
    d->customColor = PHWhiteColor;
    d->time = 1.0f;
    d->view = imageView;
    d->timeFunction = PHAnimationDescriptor::FadeOutFunction;
    PHView::addAnimation(d);
    d->release();
    
    PHParticleView * pv = new PHParticleView();
    pv->setFrame(PHRect(frame.width/2-50-450,frame.height/2-100,100,100));
    PHParticleAnimator * pa = new PHParticleAnimator;
    pa->setGravity(PHPoint(0,-10));
    pa->setParticleLifetime(3.0f);
    pa->setVelocity(PHPoint(100,200));
    pa->setInitSize(PHSize(50,50));
    pa->setFinalSize(PHSize(300,300));
    pa->setInitColor(PHWhiteColor);
    pa->setFinalColor(PHClearColor);
    pa->setSpreadAngle(M_PI/6);
    pa->setParticlesPerSecond(10);
    pa->setAnimatorPool(PHAnimatorPool::currentAnimatorPool());
    pv->setParticleAnimator(pa);
    pv->setImage(PHImage::imageNamed("argo"));
    pv->setBackgroundColor(PHWhiteColor);
    v->addSubview(pv);

    pv = new PHParticleView();
    pv->setFrame(PHRect(frame.width/2-50+500,frame.height/2-100,100,100));
    pa = new PHParticleAnimator;
    pa->setGravity(PHPoint(0,-10));
    pa->setParticleLifetime(3.0f);
    pa->setVelocity(PHPoint(-100,200));
    pa->setInitSize(PHSize(50,50));
    pa->setFinalSize(PHSize(300,300));
    pa->setInitColor(PHWhiteColor);
    pa->setFinalColor(PHClearColor);
    pa->setSpreadAngle(M_PI/6);
    pa->setParticlesPerSecond(10);
    pa->setAnimatorPool(PHAnimatorPool::currentAnimatorPool());
    pv->setParticleAnimator(pa);
    pv->setImage(PHImage::imageNamed("argo"));
    pv->setBackgroundColor(PHWhiteColor);
    v->addSubview(pv);

//    PHTextView * tv = new PHTextView(PHRect(frame.width/2-300,frame.height/2-300,600,
    
    v->addSubview(imageView);
    
    return v;
}

void PHARGOController::updateScene(double timeElapsed)
{
  //  PHAnimatorPool::currentAnimatorPool()->advanceAnimation(<#double elapsed#>)
}