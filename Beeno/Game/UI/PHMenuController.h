//
//  PHMenuController.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 8/20/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHMENUCONTROLLER_H
#define PHMENUCONTROLLER_H

#include "PHViewController.h"

class PHImageView;
class PHMenuController : public PHViewController 
{
private:
    vector<PHImageView*> clouds;
    PHNavigationController * nav;
    
    void viewWillAppear();
    void resetClouds(PHView * v);
protected:
    virtual PHView * loadView(const PHRect & frame);
    virtual void updateScene(ph_float timeElapsed);
};

#endif
