/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHGameController.h"
#include <Porkholt/Core/PHGLLight.h>
#include <Porkholt/Core/PHDeferredView.h>
#include "PHGameView.h"
#include "PHPlayerView.h"
#include <Porkholt/Core/PHImageView.h>

#define screenHeight 4.0f
PHRect PHGameController::playerRect(-0.5,-0.5,1,1);

PHView * PHGameController::loadView(const PHRect & frame)
{
    PHDeferredView * v = defferedView = new PHDeferredView(frame);
    defferedView->setUserInput(true);
    defferedView->setAmbientColor(PHColor(0.7f,0.7f,0.7f));
    v->retain();
    gameView = new PHGameView(frame);
    gameView->setUserInput(true);
    gameView->setBounds(PHRect(0,0,screenHeight*(frame.width/frame.height),screenHeight));
    gameView->setLeftTouch(PHPoint(0, screenHeight/2));
    gameView->setRightTouch(PHPoint(gameView->bounds().width, screenHeight/2));
    v->addChild(gameView);
    
    backgroundView = new PHImageView();
    backgroundView->setImage(gm->imageNamed("background"));
    backgroundView->setRepeatX(2);
    
    gameView->addChild(backgroundView);
    gameView->addChild(playerView = new PHPlayerView(playerRect));
    
    ph_float ratio = frame.height/screenHeight;
    PHGLLight * l = new PHGLLight(PHGLLight::pointLight, PHVector3(0,0,ratio), PHWhiteColor, ratio);
    attachLightToView(l, playerView);
    l->release();
    
    reset();
    
    return v;
}

PHGameController::~PHGameController()
{
    for (map<PHView*,PHGLLight*>::iterator i = lights.begin(); i!=lights.end(); i++)
    {
        PHView * v = i->first;
        PHGLLight * l = i->second;
        defferedView->removeLight(l);
        v->release();
        l->release();
    }
    playerView->release();
    gameView->release();
    defferedView->release();
}

void PHGameController::syncLights()
{
    list<PHView*> deleteList;
    for (map<PHView*,PHGLLight*>::iterator i = lights.begin(); i!=lights.end(); i++)
    {
        PHView * v = i->first;
        PHGLLight * l = i->second;
        if (!(v->superview()))
        {
            defferedView->removeLight(l);
            v->release();
            l->release();
            deleteList.push_back(v);
        } else {
            l->position = PH3DPoint(v->fromMyCoordinates(v->boundsCenter()), l->position.z);
        }
    }
    for (list<PHView*>::iterator i = deleteList.begin(); i!= deleteList.end(); i++)
        lights.erase(*i);
}

void PHGameController::detachLightFromView(PHView * v)
{
    map<PHView*,PHGLLight*>::iterator i = lights.find(v);
    if (i == lights.end()) return;
    defferedView->removeLight(i->second);
    i->second->release();
    v->release();
    lights.erase(i);
}

void PHGameController::reset()
{
    playerPos = PHPoint(1.0f,2.0f);
    gameView->setLeftTouch(playerPos);
    noise[0].clear();
    noise[1].clear();
    noise[2].clear();
    noise[3].clear();
    memset(noiseStart, 0, sizeof(noiseStart));
    memset(noiseTime, 0, sizeof(noiseTime));
    
    playerView->setPosition(playerPos+playerRect.origin());
    backgroundView->setFrame(gameView->bounds() * PHSize(2.0f,1.0f));
    generateGround();
}

void PHGameController::generateGround()
{
    ph_float max = gameView->bounds().width;
    ph_float step = 3.0f*(gameView->frame().width/max);
    for (int i=0; i<4; i++)
    {
        for (;noiseStart[i]+step<=0; noiseStart[i]+=step, noise[i].pop_front());
        for (;noiseStart[i]+step*noise[i].size()<=max; noiseTime[i]++)
            noise[i].push_back(sin(noiseTime[i]/10.0f));
    }
}

void PHGameController::attachLightToView(PHGLLight * l, PHView * v)
{
    map<PHView*,PHGLLight*>::iterator i = lights.find(v);
    if (i != lights.end()) 
    {
        l->retain();
        defferedView->removeLight(i->second);
        defferedView->addLight(l);
        i->second->release();
        i->second = l;
    } else {
        l->retain();
        v->retain();
        defferedView->addLight(l);
        lights.insert(make_pair(v, l));
    }
}

void PHGameController::updateScene(ph_float elapsedTime)
{
    //if (gameView->touchesLeft())
    {
        PHPoint pnt = gameView->leftTouch();
        PHLowPassFilter(playerPos.y, pnt.y, elapsedTime, 5.0f);
    }
    playerView->setPosition(playerPos+playerRect.origin());
    
    ph_float max = gameView->bounds().width;
    ph_float advance = elapsedTime*4.0f;
    PHPoint p = backgroundView->position();
    p.x-=advance;
    while (p.x<-max)
        p.x+=max;
    backgroundView->setPosition(p);
    
    syncLights();
}
