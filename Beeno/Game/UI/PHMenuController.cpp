/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHMenuController.h"
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHNavigationController.h>
#include <Porkholt/Core/PHViewControllerHolder.h>
#include "PHTitleScreen.h"
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Sound/PHSoundManager.h>
#include <Porkholt/Sound/PHSound.h>

static const float cloudX[] = {0.0f,1.0f,0.3f};
static const float cloudY[] = {0.5f,0.7f,0.12f};
static const float cloudW[] = {0.4f,0.4f,0.4f};
static const float cloudS[] = {0.015f,-0.01f,0.02f};
static const char * const cloudP[] = {"cloud1","cloud2","cloud3"};


PHView * PHMenuController::loadView(const PHRect & frame)
{
    PHImageView * v = new PHImageView(frame);
    v->setImage(gm->imageNamed("bg_gradient"));
    v->setUserInput(true);
    
    for(int i=0; i<(sizeof(cloudX)/sizeof(float)); i++)
    {
        PHImageView * c = new PHImageView;
        c->setImage(gm->imageNamed(cloudP[i]));
        c->setTag(i+10);
        clouds.push_back(c);
        v->addChild(c);
        c->release();
    }
    resetClouds(v);
    
    nav = new PHNavigationController(gm);
    nav->setInitialViewFrame(v->bounds());
    PHViewControllerHolder * holder = new PHViewControllerHolder(v->bounds());
    holder->setViewController(nav);
    v->addChild(holder);
    holder->release();
    nav->setNavigationController(navigationController());
    
    vc = new PHTitleScreen(gm);
    vc->setInitialViewFrame(v->bounds());
	nav->pushViewController(vc);
    vc->release();
    nav->release();
    
    return v;
}

void PHMenuController::viewWillAppear()
{
    if (nav)
        nav->_viewWillAppear();
    resetClouds(getView());
    gm->soundManager()->setBackgroundMusic(gm->soundManager()->soundNamed("title"));
}

void PHMenuController::viewDidAppear()
{
    if (nav)
        nav->_viewDidAppear();
}

void PHMenuController::viewWillDisappear()
{
    if (nav)
        nav->_viewWillDisappear();
}

void PHMenuController::viewDidDisappear()
{
    if (nav)
        nav->_viewDidDisappear();
}

void PHMenuController::resetClouds(PHView * v)
{
    float w = v->bounds().width;
    float h = v->bounds().height;
    for (int i=0; i<clouds.size(); i++)
    {
        PHImage * img = clouds[i]->image();
        float hi = ((float)img->height())/((float)img->width())*(cloudW[i]*w);
        clouds[i]->setFrame(PHRect(
                                   cloudX[i]*w-(cloudW[i]*w)/2,
                                   cloudY[i]*h-hi/2,
                                   cloudW[i]*w,
                                   hi
                            ));
    }
}

void PHMenuController::updateScene(float timeElapsed)
{
    PHView * v = getView();
    if (!v) return;
    float w = v->bounds().width;
    for (int i=0; i<clouds.size(); i++)
    {
        PHRect fr = clouds[i]->frame()+PHPoint(cloudS[i]*w*timeElapsed,0);
        if (fr.x>=w)
            fr.x=-fr.width;
        else
        if (fr.x<=-fr.width)
            fr.x=w;
        clouds[i]->setFrame(fr);
    }
}
