//
//  PHMenuController.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 8/20/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHMenuController.h"
#include "PHView.h"
#include "PHNavigationController.h"
#include "PHTitleScreen.h"
#include "PHImageView.h"
#include "PHMusicManager.h"

static const ph_float cloudX[] = {0.0f,1.0f,0.3f};
static const ph_float cloudY[] = {0.5f,0.7f,0.12f};
static const ph_float cloudW[] = {0.4f,0.4f,0.4f};
static const ph_float cloudS[] = {0.015f,-0.01f,0.02f};
static const char * const cloudP[] = {"cloud1","cloud2","cloud3"};


PHView * PHMenuController::loadView(const PHRect & frame)
{
    PHImageView * v = new PHImageView(frame);
    v->setImage(_gameManager->imageNamed("bg_gradient"));
    v->setUserInput(true);
    
    for(int i=0; i<(sizeof(cloudX)/sizeof(ph_float)); i++)
    {
        PHImageView * c = new PHImageView;
        c->setImage(_gameManager->imageNamed(cloudP[i]));
        c->setTag(i+10);
        clouds.push_back(c);
        v->addSubview(c);
        c->release();
    }
    resetClouds(v);
    
    nav = new PHNavigationController();
    nav->init(_gameManager,frame);
    nav->_viewWillAppear();
    v->addSubview(nav->getView());
    nav->_viewDidAppear();
    manageViewController(nav);
    
    PHViewController * vc = new PHTitleScreen();
	vc->init(_gameManager,v->bounds());
	nav->pushViewController(vc);
    vc->release();
    nav->release();
    
    return v;
}

void PHMenuController::viewWillAppear()
{
    resetClouds(getView());
    PHMusicManager::singleton()->setBackgroundMusic(_gameManager->musicNamed("title"));
}

void PHMenuController::resetClouds(PHView * v)
{
    ph_float w = v->bounds().width;
    ph_float h = v->bounds().height;
    for (int i=0; i<clouds.size(); i++)
    {
        PHImage * img = clouds[i]->image();
        ph_float hi = ((ph_float)img->height())/((ph_float)img->width())*(cloudW[i]*w);
        clouds[i]->setFrame(PHRect(
                                   cloudX[i]*w-(cloudW[i]*w)/2,
                                   cloudY[i]*h-hi/2,
                                   cloudW[i]*w,
                                   hi
                            ));
    }
}

void PHMenuController::updateScene(ph_float timeElapsed)
{
    PHView * v = getView();
    if (!v) return;
    ph_float w = v->bounds().width;
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