/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHGAMECONTROLLER_H
#define PHGAMECONTROLLER_H

#include <Porkholt/Core/PHViewController.h>

class PHView;
class PHGLLight;
class PHDeferredView;
class PHGameView;
class PHPlayerView;
class PHImageView;

class PHGameController : public PHViewController
{
protected:
    PHPoint playerPos;
    PHDeferredView * defferedView;
    PHGameView * gameView;
    PHPlayerView * playerView;
    PHImageView * backgroundView;
    
    PHView * loadView(const PHRect & frame);
    void updateScene(ph_float elapsedTime);
    ~PHGameController();
    
    map<PHView*,PHGLLight*> lights;
    
    
    static PHRect playerRect;
    
    list<ph_float> noise[4];
    ph_float noiseStart[4];
    int noiseTime[4];
    void generateGround();
    void syncLights();
    
public:
    void attachLightToView(PHGLLight * l, PHView * v);
    void detachLightFromView(PHView * v);
    
    void reset();

    PHGameController(PHGameManager * gm) : PHViewController(gm) {}
};


#endif
