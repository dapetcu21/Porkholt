/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMENUCONTROLLER_H
#define PHMENUCONTROLLER_H

#include "PHViewController.h"

class PHImageView;
class PHTitleScreen;
class PHMenuController : public PHViewController 
{
private:
    vector<PHImageView*> clouds;
    PHNavigationController * nav;
    PHTitleScreen * vc;
    
    void viewWillAppear();
    void resetClouds(PHView * v);
protected:
    virtual PHView * loadView(const PHRect & frame);
    virtual void updateScene(ph_float timeElapsed);
    
public:
    PHTitleScreen * titleScreen() { getView(); return vc; }
};

#endif
